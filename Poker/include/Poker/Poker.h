#pragma once
#include <iostream>
#include <algorithm>
#include <list>
#include <vector>
#include <optional>
#include <unordered_map>
#include <random>
#include <string>
#include <sstream>
#include "stb_image.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>\

enum class Figure : uint8_t {Hearts = 4, Diamonds = 3, Spades = 2, Clubs = 1, None = 0};
enum class Rank : uint8_t {Two=2, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace, Blank};
enum class Rates : uint8_t {None, HighCard, OnePair, TwoPair, TOTK, Straight, Flush, FullHouse, FOTK, StraightFlush, RoyalFlush};
enum class Phase : int { PreFlop, Flop, Turn, River };

static std::string RankLookUpTable[13] = { "Two","Three","Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King", "Ace"};
static std::string RatesLookUpTable[13] = { "None","HighCard","OnePair", "TwoPair", "TOTK", "Straight", "Flush", "FullHouse", "FOTK", "StraightFlush", "RoyalFlush" };
inline std::ostream& operator<<(std::ostream& os, const Rank& rank)
{
	os << RankLookUpTable[(int)rank - 2];
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Figure& figure)
{
	switch (figure)
	{
	case Figure::Hearts:
		os << "Hearts";
		break;
	case Figure::Diamonds:
		os << "Diamons";
		break;
	case Figure::Spades:
		os << "Spades";
		break;
	case Figure::Clubs:
		os << "Clubs";
		break;
	case Figure::None:
		os << "None";
		break;
	}
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const Rates& rate)
{
	os << RatesLookUpTable[(int)rate];
	return os;
}

struct Card {
	Figure figure;
	Rank rank;
	operator int() const { return (int)rank * 5 + (int)rank; }
	bool operator >(const Card& card) const { return (int)*this > (int)card; }
	bool operator <(const Card& card) const { return (int)*this < (int)card; }
	const bool operator ==(const Card& card) const { return figure == card.figure && rank == card.rank; }
	Card operator +(int i) const
	{
		Rank result;
		if ((int)rank + i <= 14 && (int)rank + i >= 2)
			result = Rank((int)rank + i);
		else if ((int)rank + i < 14)
			result = Rank(2);
		else
			result = Rank(14);
		return { figure, result };
	}
	Card operator +(const Card& card) const 
	{ 
		Rank result;
		if ((int)rank + (int)card.rank <= 14 && (int)rank + (int)card.rank >= 2)
			result = Rank((int)rank + (int)card.rank);
		else if ((int)rank + (int)card.rank < 14)
			result = Rank(2);
		else
			result = Rank(14);
		return { figure, result };
	}
	friend std::ostream& operator<<(std::ostream& os, const Card& card);
};
struct Hand {
	Card first_card;
	Card second_card;
	const Card& GetLowCard() const { return first_card < second_card ? first_card : second_card; }
	const Card& GetHighCard() const { return first_card > second_card ? first_card : second_card; }
	Card& GetLowCard() { return first_card < second_card ? first_card : second_card; }
	Card& GetHighCard() { return first_card > second_card ? first_card : second_card; }
	Card& operator [](int i) { switch (i) { case 0: return first_card; break; case 1: return second_card; break; } }
	const Card& operator [](int i) const { switch (i) { case 0: return first_card; break; case 1: return second_card; break; } }
};
struct CardValue {
	Rates rate;
	Card weight_card;
	static bool Compare(const CardValue& a, const CardValue& b) {
		if (a.rate != b.rate)
			return a.rate > b.rate;
		return a.weight_card > b.weight_card;
	}
	bool operator >(const CardValue& b) const
	{
		if (rate != b.rate)
			return rate > b.rate;
		return weight_card > b.weight_card;
	}
	bool operator <(const CardValue& b) const
	{
		if (rate != b.rate)
			return rate < b.rate;
		return weight_card < b.weight_card;
	}
	bool operator ==(const CardValue& b) const
	{
		if (rate == b.rate && weight_card == b.weight_card)
			return 1;
		return 0;
	}
	bool operator !=(const CardValue& b) const
	{
		if (rate != b.rate || weight_card != b.weight_card)
			return 1;
		return 0;
	}
	std::string ToString()
	{
		std::stringstream ss;
		ss << rate << " (";
		ss << weight_card.rank;
		if (weight_card.figure != Figure::None)
		{
			ss << " of ";
			ss << weight_card.figure;
		}
		ss << ")";
		return  ss.str();
	}
	CardValue() : rate(Rates::None), weight_card(Figure::Hearts, Rank::Two) { }
	CardValue(Rates rate, Card weight_card) : rate(rate), weight_card(weight_card) { }
	CardValue(Rates rate, Rank rank) : rate(rate), weight_card(Figure::Diamonds, rank) { }
};
inline std::ostream& operator<<(std::ostream& os, const Card& card)
{
	os << card.rank << " of " << card.figure;
	return os;
}
namespace std {
	template<>
	struct hash<Card>
	{
		std::size_t operator()(const Card& card) const noexcept
		{
			return (static_cast<std::size_t>(card.figure) << 8) ^ static_cast<std::size_t>(card.rank);
		}
	};
}
class Image {
public:
	int width = 0;
	int height = 0;
	GLuint texture = -1;

	Image() {}
	Image(const char* file_name) { LoadTextureFromFile(file_name); }

	void LoadTextureFromMemory(const void* data, size_t data_size)
	{
		unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &width, &height, NULL, 4);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
		stbi_image_free(image_data);
	}
	void LoadTextureFromFile(const char* file_name)
	{
		FILE* f = fopen(file_name, "rb");
		if (f == NULL)
		{
			std::cout << "Failed to open file: " << file_name << "\n";
			return;
		}
		fseek(f, 0, SEEK_END);
		size_t file_size = (size_t)ftell(f);
		fseek(f, 0, SEEK_SET);
		void* file_data = IM_ALLOC(file_size);
		fread(file_data, 1, file_size, f);
		fclose(f);
		LoadTextureFromMemory(file_data, file_size);
		IM_FREE(file_data);
	}
};
class Player
{
	std::string nick;
	Hand hand{Figure::Hearts, Rank::Two, Figure::Clubs, Rank::Four};
	int fullBet = 0;
public:
	Player() {}
	Player(std::string nick) : nick(nick) {}
	operator const std::string& () { return nick; }
	std::string& GetNick() { return this->nick; }
	const std::string& GetNick() const { return this->nick; }
	Hand& GetHand() { return hand; }
	const Hand& GetHand() const { return hand; }
	void Bet(int bet) {}
	void Fold() {}
	void Call() {}
};
class Poker
{
	std::vector<Card> m_PoolSet;
	std::vector<Player> m_PlayerPool;
	Phase m_Phase = Phase::PreFlop;
	Card m_Table[5]{};
	int maxBet = 0;
public:
	void NewGame();
	void Repool();
	std::string WhoWon();
	std::vector<std::pair<Player, CardValue>> GetResultList();
	const std::vector<Player>& GetPlayerPool();
	std::vector<CardValue> EvaluateHand(const Hand& hand);
	CardValue EvaluateBestHand(const Hand& hand);
	void EvaluateHands();
	Hand DealHand();
	void DealHands();
	void DealHandToPlayer(const std::string& nick);
	void DealHandToPlayer(Player& player);
	void DealHandToPlayer(const std::string& nick, const Hand& hand);
	void RemoveCard(int index);
	Card PopCard(int index);
	Card PopRandomCard();
	int AddPlayer(const std::string& nick);
	int RemovePlayer(const std::string& nick);
	std::optional<Player*> FindPlayer(const std::string& nick);
	std::vector<Player>& GetPlayers();
	Phase& GetPhase();
	void RefillTable();
	Card& GetTableCard(int i);
	void PrintCardPool();
	void PrintPlayers();
	void PrintTable();
	std::unordered_map<Rank, int> CreateRankMap(const Hand& hand);
	std::unordered_map<Card, int> CreateCardMap(Hand hand);
	CardValue isMinCard(const Hand& hand);
	CardValue isHighCard(const Hand& hand);
	std::vector<CardValue> isPair(const Hand& hand, const std::unordered_map<Rank, int>& pairs);
	std::vector<CardValue> isTOTK(const Hand& hand, const std::unordered_map<Rank, int>& pairs);
	CardValue isStraight(const Hand& hand);
	CardValue isFlush(const Hand& hand, std::unordered_map<Rank, int>& pairs);
	CardValue isFullHouse(const Hand& hand, std::unordered_map<Rank, int>& pairs);
	CardValue isFOTK(const Hand& hand, const std::unordered_map<Rank, int>& map);
	CardValue isStraightFlush(const Hand& hand, std::unordered_map<Rank, int>& map, std::unordered_map<Card, int>& cardMap);
	CardValue isRoyalFlush(const Hand& hand, std::unordered_map<Card, int>& map);
};
struct CardCrop {
	Card card;
	float x, y, w, h;
	CardCrop() : x(0), y(0), w(24), h(32), card() {}
	CardCrop(const Card& card) { SetNewCard(card); }
	CardCrop(Poker& game, int i) { SetNewTableCard(game, i); }
	void SetNewCard(const Card& card)
	{
		this->card = card;
		x = 1.0f / 14 * ((int)card.rank - 2);
		y = 0.25f * (4 - (int)card.figure);
		w = x + 1.0f / 14;
		h = y + 0.25f;
	}
	void SetNewTableCard(Poker& game, int i)
	{
		card = game.GetTableCard(i);
		x = 1.0f / 14 * ((int)card.rank - 2);
		y = 0.25f * (4 - (int)card.figure);
		w = x + 1.0f / 14;
		h = y + 0.25f;
	}
	void Print(const char* id,const Image& cards_textures, const Card& card, int size = 6)
	{
		if (this->card != card)
		{
			SetNewCard(card);
			this->card = card;
		}
		ImGui::ImageButton(id,(ImTextureID)(intptr_t)cards_textures.texture, ImVec2(24.0f * size, 32.0f * size), ImVec2(x, y), ImVec2(w, h));
	}
	static void s_Print(const char* id, const Image& cards_textures, const Card& card, int size = 6)
	{
		float s_x = 1.0f / 14 * ((int)card.rank - 2);
		float s_y = 0.25f * (4 - (int)card.figure);
		float s_w = s_x + 1.0f / 14;
		float s_h = s_y + 0.25f;
		ImGui::ImageButton(id, (ImTextureID)(intptr_t)cards_textures.texture, ImVec2(24.0f * size, 32.0f * size), ImVec2(s_x, s_y), ImVec2(s_w, s_h));
	}
};
