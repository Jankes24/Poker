#include <Poker/Poker.h>
#include <chrono>
#include <thread>

void Poker::NewGame()
{
	Repool();
	DealHands();
	RefillTable();
}
void Poker::DealHands() {
	for (Player& player : m_PlayerPool)
		player.GetHand() = DealHand();
}
void Poker::Repool() {
	m_PoolSet.clear();
	for (int i = 0; i < 4; i++)
		for (int j = 2; j < 15; j++)
			m_PoolSet.emplace_back(Figure(i), Rank(j));
}
void Poker::PrintCardPool() {
	std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-\n";
	Card prevCard{};
	prevCard.figure = Figure::Hearts;
	std::cout << "\x1b[1;31mHearts\x1b[1;39m:\n";
	for (Card& card : m_PoolSet)
	{
		if (card.figure != prevCard.figure)
			std::cout << "\n-=-=-=-=-=-=-=-=-=-=-=-=-\n" << card.figure << ":\n" << card.rank << " - ";
		else
			std::cout << card.rank << " - ";
		prevCard = card;
	}
	std::cout << "\n-=-=-=-=-=-=-=-=-=-=-=-=-\n";
}
void Poker::RemoveCard(int index)
{
	m_PoolSet.erase(m_PoolSet.begin() + index);
}
Card Poker::PopCard(int index)
{
	Card result = m_PoolSet[index];
	std::cout << "Popped: " << result << "\n";
	m_PoolSet.erase(m_PoolSet.begin() + index);
	return result;
}
Card Poker::PopRandomCard()
{
	if (m_PoolSet.size() == 0)
	{
		std::cout << "No cards left in the pool, repooling...\n";
		Repool();
	}
	return PopCard(std::rand() % m_PoolSet.size());
}
Hand Poker::DealHand()
{
	return Hand(PopRandomCard(), PopRandomCard());
}
void Poker::DealHandToPlayer(const std::string& nick)
{
	std::optional<Player*> player = FindPlayer(nick);
	if (player.has_value())
		player.value()->GetHand() = DealHand();
}
void Poker::DealHandToPlayer(const std::string& nick, const Hand& hand)
{
	std::optional<Player*> player = FindPlayer(nick);
	if (player.has_value())
		player.value()->GetHand() = hand;
}
int Poker::AddPlayer(const std::string& nick)
{
	for (Player& player : m_PlayerPool)
	{
		if (player.GetNick() == nick)
		{
			std::cout << "Can't add: " << nick << " because of the name colision\n";
			return 1;
		}
	}
	m_PlayerPool.emplace_back(nick);
	std::cout << nick << " Joined!\n";
	return 0;
}
int Poker::RemovePlayer(const std::string& nick)
{
	for (int i = 0; i < m_PlayerPool.size(); i++)
	{
		if (m_PlayerPool[i].GetNick() == nick)
		{
			m_PlayerPool.erase(m_PlayerPool.begin() + i);
			std::cout << nick << " Removed!\n";
			return 0;
		}
	}
	std::cout << "Can't remove: " << nick << " he doesn't exist\n";
	return 1;
}
std::optional<Player*> Poker::FindPlayer(const std::string& nick)
{
	for (int i = 0; i < m_PlayerPool.size(); i++)
	{
		if (m_PlayerPool[i].GetNick() == nick)
		{
			std::cout << nick << " Found!\n";
			return &m_PlayerPool[i];
		}
	}
	std::cout << "Can't find: " << nick << " he doesn't exist\n";
	return {};
}
void Poker::PrintPlayers()
{
	std::cout << "-=-=-=-=-=-=- PLAYERS: -=-=-=-=-=-=-\n";
	int i = 0;
	for (Player& player : m_PlayerPool)
	{
		i++;
		std::cout << i << ". " << player.GetNick() << " - " << player.GetHand().first_card << ", " << player.GetHand().second_card << "\n";
	}
	std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-\n";
}
void Poker::PrintTable()
{
	std::cout << "-=-=-=-=-=-=- TABLE: -=-=-=-=-=-=-\n";
	for (int i = 0; i < 5; i++)
		std::cout << m_Table[i] << " - ";
	std::cout << "\n-=-=-=-=-=-=-=-=-=-=-=-=-\n";
}
void Poker::RefillTable()
{
	for (int i = 0; i < 5; i++)
		m_Table[i] = PopRandomCard();
}
Card& Poker::GetTableCard(int i)
{
	return m_Table[i];
}
std::string Poker::WhoWon()
{
	return GetResultList()[0].GetNick();
}
std::vector<Player> Poker::GetResultList()
{
	std::vector<Player> resultList;
	std::vector <std::pair<std::vector<CardValue>, Player>> players;
	for (const auto& player : m_PlayerPool)
	{
		std::pair<std::vector<CardValue>, Player> pair;
		pair.second = player;
		for (const auto& value : EvaluateHand(player.GetHand()))
		{
			pair.first.emplace_back(value);
		}
		players.emplace_back(pair);
	}
	std::sort(players.begin(), players.end(), [](const auto& a, const auto& b) {
		for (int i = 0; i < std::min(a.first.size(), b.first.size()); i++)
			if (a.first[i] != b.first[i])
				return a.first[i] > b.first[i];
		return false;
		});
	for (const auto& i : players)
		resultList.emplace_back(i.second);
	return resultList;
}
void Poker::EvaluateHands()
{
	std::cout << "-=-=-=-=-=- HANDS: -=-=-=-=-=-=-=-\n";
	for (const auto& player : m_PlayerPool)
	{
		std::cout << player.GetNick() << ": ";
		for (const auto& value : EvaluateHand(player.GetHand()))
		{
			if (value.rate != Rates::None)
				std::cout << value.rate << " (" << value.weight_card << "), ";
		}
		std::cout << "\n-=-=-=-=-=-=-=-=-=-=-=-=-\n";
	}
}
std::unordered_map<Rank, int> Poker::CreateRankMap(const Hand& hand)
{
	std::unordered_map<Rank, int> map;
	map[hand[0].rank]++;
	map[hand[1].rank]++;
	for (int i = 0; i < 5; i++)
		map[m_Table[i].rank]++;
	return map;
}
std::vector<CardValue> Poker::EvaluateHand(const Hand& hand)
{
	std::vector<CardValue> result;
	std::unordered_map<Rank, int> rankMap = CreateRankMap(hand);
	CardValue straight = isStraight(hand);
	CardValue flush = isFlush(hand, rankMap);
	result.emplace_back(isMinCard(hand));
	result.emplace_back(isHighCard(hand));

	for (const auto& val : isPairs(hand, rankMap))
		result.emplace_back(val);

	for (const auto& val : isTOTK(hand, rankMap))
		result.emplace_back(val);

	result.emplace_back(straight);
	result.emplace_back(flush);
	result.emplace_back(isFullHouse(hand, rankMap));
	result.emplace_back(isFOTK(hand, rankMap));

	if (straight.rate == Rates::Straight && flush.rate == Rates::Flush)
		result.emplace_back(isRoyalFlush(hand, rankMap));
	
	std::sort(result.begin(), result.end(), CardValue::Compare);
	return result;
}

CardValue Poker::isMinCard(const Hand& hand)
{
	return CardValue(Rates::HighCard, hand.GetLowCard());
}

CardValue Poker::isHighCard(const Hand& hand)
{
	return CardValue(Rates::HighCard, hand.GetHighCard());
}

std::vector<CardValue> Poker::isPairs(const Hand& hand, const std::unordered_map<Rank, int>& map)
{
	std::vector<CardValue> result;
	for (const auto& pri : map)
		if (pri.second == 2)
			result.emplace_back(Rates::OnePair, pri.first);
	if (result.size() >= 2)
		for (auto& cv : result)
			cv.rate = Rates::TwoPair;
	return result;
}

std::vector<CardValue> Poker::isTOTK(const Hand& hand, const std::unordered_map<Rank, int>& map)
{
	std::vector<CardValue> result;
	for (const auto& pri : map)
		if (pri.second == 3)
			result.emplace_back(Rates::TOTK, pri.first);
	return result;
}

CardValue Poker::isStraight(const Hand& hand)
{
	std::unordered_map<Figure, int> figureMap;
	figureMap[hand[0].figure]++;
	figureMap[hand[1].figure]++;
	for (int i = 0; i < 5; i++)
		figureMap[m_Table[i].figure]++;

	for (const auto& i : figureMap)
		if (i.second >= 5) return { Rates::Straight, {hand.GetHighCard().figure == i.first ? hand.GetHighCard() : hand.GetLowCard()}};

	return CardValue();
}

CardValue Poker::isFlush(const Hand& hand, std::unordered_map<Rank, int>& pairs)
{
	for (int i = 0; i < 2; i++)
	{
		int length = 1;
		for (int j = 1; j < 7; j++)
			if (pairs[(hand[i] + -j).rank] && (int)hand[i].rank - j >= 2 && (int)hand[i].rank + j <= 14) length++;
			else if ((int)hand[i].rank - j == 1) length++;
			else break;
		int j = 1;
		for (; j < 7; j++)
			if (pairs[(hand[i] + j).rank] && (int)hand[i].rank - j >= 2 && (int)hand[i].rank + j <= 14) length++;
			else break;
		if (length >= 5)
			return { Rates::Flush, hand[i] + j + -1 };
	}
	return CardValue();
}

CardValue Poker::isFullHouse(const Hand& hand, std::unordered_map<Rank, int>& pairs)
{
	for (const auto& pr : pairs)
		if (pairs[pr.first] >= 3)
			for (const auto& pri : pairs)
				if (pairs[pri.first] >= 2 && pri != pr) 
					if (pr.first == hand[0].rank || pri.first == hand[0].rank && pr.first == hand[1].rank || pri.first == hand[1].rank)
						return { Rates::FullHouse,  hand[hand[1] > hand[0]] };
					else if (pr.first == hand[1].rank || pri.first == hand[1].rank)
						return { Rates::FullHouse,  hand[1] };
					else if (pr.first == hand[0].rank || pri.first == hand[0].rank)
						return { Rates::FullHouse,  hand[1] };
					else
						return { Rates::FullHouse,  pr.first };
	return CardValue();
}

CardValue Poker::isFOTK(const Hand& hand, const std::unordered_map<Rank, int>& map)
{
	std::vector<CardValue> result;
	for (const auto& pri : map)
		if (pri.second == 4)
			return { Rates::FOTK, pri.first };
	return CardValue();
}

CardValue Poker::isStraightFlush(const Hand& hand, std::unordered_map<Rank, int>& map)
{
	for (int i = 0; i < 2; i++)
	{
		int length = 1;
		for (int j = 1; j < 7; j++)
			if (map[(hand[i] + -j).rank] && (int)hand[i].rank - j >= 2 && (int)hand[i].rank + j <= 14 && (hand[i] + -j).figure == hand[i].figure) length++;
			else if ((int)hand[i].rank - j == 1) length++;
			else break;
		int j = 1;
		for (; j < 7; j++)
			if (map[(hand[i] + j).rank] && (int)hand[i].rank - j >= 2 && (int)hand[i].rank + j <= 14 && (hand[i] + j).figure == hand[i].figure) length++;
			else break;
		if (length >= 5)
			return { Rates::StraightFlush, hand[i] + j + -1 };
	}
	return CardValue();
}

CardValue Poker::isRoyalFlush(const Hand& hand, std::unordered_map<Rank, int>& map)
{
	CardValue royalFlush = isStraightFlush(hand, map);
	if (royalFlush.rate == Rates::StraightFlush && royalFlush.weight_card == Card(Figure::Hearts, Rank::Ace) && map[Rank::King] && map[Rank::Queen] && map[Rank::Jack] && map[Rank::Ten])
		return { Rates::RoyalFlush, royalFlush.weight_card };
	else
		return royalFlush;
	return CardValue();
}
