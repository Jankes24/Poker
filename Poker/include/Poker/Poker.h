#pragma once
#include <iostream>
#include <algorithm>
#include <list>
#include <vector>
#include <optional>
#include <unordered_map>
#include <random>

enum class Figure : uint8_t {Hearts = 3, Diamonds = 2, Spades = 1, Clubs = 0};
enum class Rank : uint8_t {Two=2, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace};
enum class Rates : uint8_t {None, HighCard, OnePair, TwoPair, TOTK, Straight, Flush, FullHouse, FOTK, StraightFlush, RoyalFlush};

struct Card {
	Figure figure;
	Rank rank;
	operator int() const { return (int)rank * 4 + (int)rank; }
	bool operator >(const Card& card) const { return (int)*this > (int)card; }
	bool operator <(const Card& card) const { return (int)*this < (int)card; }
	const bool operator ==(const Card& card) const { return (int)*this == (int)card; }
	const Card& operator +(int i) const
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
	CardValue() : rate(Rates::None), weight_card(Figure::Hearts, Rank::Two) { }
	CardValue(Rates rate, Card weight_card) : rate(rate), weight_card(weight_card) { }
	CardValue(Rates rate, Rank rank) : rate(rate), weight_card(Figure::Diamonds, rank) { }
};

static std::string RankLookUpTable[13] = { "Two","Three","Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King", "Ace" };
static std::string RatesLookUpTable[13] = { "None","HighCard","OnePair", "TwoPair", "TOTK", "Straight", "Flush", "FullHouse", "FOTK", "StraightFlush", "RoyalFlush" };
static std::ostream& operator<<(std::ostream& os, const Rank& rank)
{
	os << RankLookUpTable[(int)rank - 2];
	return os;
}
static std::ostream& operator<<(std::ostream& os, const Figure& figure)
{
	switch (figure)
	{
	case Figure::Hearts:
		os << "\x1b[1;31mHearts\x1b[1;39m";
		break;
	case Figure::Diamonds:
		os << "\x1b[1;31mDiamons\x1b[1;39m";
		break;
	case Figure::Spades:
		os << "\x1b[1;37mSpades\x1b[1;39m";
		break;
	case Figure::Clubs:
		os << "\x1b[1;37mClubs\x1b[1;39m";
		break;
	}
	return os;
}
static std::ostream& operator<<(std::ostream& os, const Card& card)
{
	os << card.rank << " of " << card.figure;
	return os;
}
static std::ostream& operator<<(std::ostream& os, const Rates& rate)
{
	os << RatesLookUpTable[(int)rate];
	return os;
}


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
	Card m_Table[5];
	int maxBet = 0;
public:
	void NewGame();
	void Repool();
	std::string WhoWon();
	std::vector<Player> GetResultList();
	std::vector<CardValue> EvaluateHand(const Hand& hand);
	void EvaluateHands();
	Hand DealHand();
	void DealHands();
	void DealHandToPlayer(const std::string& nick);
	void DealHandToPlayer(const std::string& nick, const Hand& hand);
	void RemoveCard(int index);
	Card PopCard(int index);
	Card PopRandomCard();
	int AddPlayer(const std::string& nick);
	int RemovePlayer(const std::string& nick);
	std::optional<Player*> FindPlayer(const std::string& nick);
	void RefillTable();
	Card& GetTableCard(int i);
	void PrintCardPool();
	void PrintPlayers();
	void PrintTable();
	std::unordered_map<Rank, int> CreateRankMap(const Hand& hand);
	CardValue isMinCard(const Hand& hand);
	CardValue isHighCard(const Hand& hand);
	std::vector<CardValue> isPairs(const Hand& hand, const std::unordered_map<Rank, int>& pairs);
	std::vector<CardValue> isTOTK(const Hand& hand, const std::unordered_map<Rank, int>& pairs);
	CardValue isStraight(const Hand& hand);
	CardValue isFlush(const Hand& hand, std::unordered_map<Rank, int>& pairs);
	CardValue isFullHouse(const Hand& hand, std::unordered_map<Rank, int>& pairs);
	CardValue isFOTK(const Hand& hand, const std::unordered_map<Rank, int>& map);
	CardValue isStraightFlush(const Hand& hand, std::unordered_map<Rank, int>& map);
	CardValue isRoyalFlush(const Hand& hand, std::unordered_map<Rank, int>& map);
};