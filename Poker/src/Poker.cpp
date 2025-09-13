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
	for (int i = 1; i < 5; i++)
		for (int j = 2; j < 15; j++)
			m_PoolSet.emplace_back(Figure(i), Rank(j));
}
void Poker::PrintCardPool() {
	Card prevCard = *m_PoolSet.begin();
	std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-\n" << prevCard.figure << ": " << prevCard.rank << " - ";
	for (Card& card : m_PoolSet)
	{
		if (card.figure != prevCard.figure)
			std::cout << "\n-=-=-=-=-=-=-=-=-=-=-=-=-\n" << card.figure << ": " << card.rank << " - ";
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
void Poker::DealHandToPlayer(Player& player)
{
	player.GetHand() = DealHand();
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
std::vector<Player>& Poker::GetPlayers()
{
	return m_PlayerPool;
}
Phase& Poker::GetPhase()
{
	return m_Phase;
}
const std::vector<Player>& Poker::GetPlayerPool() {
	return m_PlayerPool;
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
	return GetResultList()[0].first.GetNick();
}
std::vector<std::pair<Player, CardValue>> Poker::GetResultList()
{
	std::vector<std::pair<Player, CardValue>> resultList;
	std::vector<std::pair<Player, std::vector<CardValue>>> players;
	
	for (const auto& player : m_PlayerPool)
	{
		std::pair<Player, std::vector<CardValue>> pair;
		pair.first = player;
		for (const auto& value : EvaluateHand(player.GetHand()))
		{
			pair.second.emplace_back(value);
		}
		players.emplace_back(pair);
	}
	std::sort(players.begin(), players.end(), [](const auto& a, const auto& b) {
		for (int i = 0; i < std::min(a.second.size(), b.second.size()); i++)
			if (a.second[i] != b.second[i])
				return a.second[i] > b.second[i];
		return false;
	});
	for (const auto& i : players)
		for (const auto& value : i.second)
		if (value.rate != Rates::None)
		{
			resultList.emplace_back(i.first, value);
			break;
		}
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
				if (value.weight_card.figure != Figure::None)
					std::cout << value.rate << " (" << value.weight_card << "), ";
				else
					std::cout << value.rate << " (" << value.weight_card.rank << "), ";
		}
		std::cout << "\n-=-=-=-=-=-=-=-=-=-=-=-=-\n";
	}
}
std::unordered_map<Rank, int> Poker::CreateRankMap(const Hand& hand)
{
	std::unordered_map<Rank, int> map;
	map[hand[0].rank]++;
	map[hand[1].rank]++;
	if (m_Phase == Phase::PreFlop) return map;
	for (int i = 0; i < 2 + (int)m_Phase; i++)
		map[m_Table[i].rank]++;
	return map;
}
std::unordered_map<Card, int> Poker::CreateCardMap(Hand hand)
{
	std::unordered_map<Card, int> map;
	map[hand[0]]++;
	map[hand[1]]++;
	if (m_Phase == Phase::PreFlop) return map;
	for (int i = 0; i < 2 + (int)m_Phase; i++)
		map[m_Table[i]]++;
	return map;
}
std::vector<CardValue> Poker::EvaluateHand(const Hand& hand)
{
	std::vector<CardValue> result;
	std::unordered_map<Rank, int> rankMap = CreateRankMap(hand);
	std::unordered_map<Card, int> cardMap = CreateCardMap(hand);

	result.emplace_back(isMinCard(hand));
	result.emplace_back(isHighCard(hand));

	for (const auto& val : isPair(hand, rankMap))
		result.emplace_back(val);

	for (const auto& val : isTOTK(hand, rankMap))
		result.emplace_back(val);

	result.emplace_back(isFullHouse(hand, rankMap));
	result.emplace_back(isStraight(hand));
	result.emplace_back(isFlush(hand, rankMap));
	result.emplace_back(isFOTK(hand, rankMap));
	result.emplace_back(isStraightFlush(hand,rankMap,cardMap));
	result.emplace_back(isRoyalFlush(hand, cardMap));
	
	std::sort(result.begin(), result.end(), CardValue::Compare);
	return result;
}
CardValue Poker::EvaluateBestHand(const Hand& hand)
{
	for (const auto& value : EvaluateHand(hand))
		if (value.rate != Rates::None)
			return value;
	return CardValue();
}
CardValue Poker::isMinCard(const Hand& hand)
{
	return CardValue(Rates::HighCard, hand.GetLowCard());
}
CardValue Poker::isHighCard(const Hand& hand)
{
	return CardValue(Rates::HighCard, hand.GetHighCard());
}
std::vector<CardValue> Poker::isPair(const Hand& hand, const std::unordered_map<Rank, int>& map)
{
	std::vector<CardValue> result;
	for (const auto& pri : map)
		if (pri.second == 2)
			result.emplace_back(Rates::OnePair, Card{ Figure::None, pri.first });
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
			result.emplace_back(Rates::TOTK, Card{ Figure::None, pri.first });
	return result;
}
CardValue Poker::isStraight(const Hand& hand)
{
	int figureCounter[5]{};
	figureCounter[(int)hand[0].figure]++;
	figureCounter[(int)hand[1].figure]++;
	for (int i = 0; i < 5; i++)
		figureCounter[(int)m_Table[i].figure]++;

	for (int i = 1; i < 5; i++)
		if (figureCounter[i] >= 5) return { Rates::Straight, hand.GetHighCard().figure == (Figure)i ? hand.GetHighCard() : hand.GetLowCard().figure == (Figure)i ? hand.GetLowCard() : Card() };

	return CardValue();
}
CardValue Poker::isFlush(const Hand& hand, std::unordered_map<Rank, int>& pairs)
{
	for (int i = 0; i < 2; i++)
	{
		int length = 1;
		for (int j = 1; j < 8; j++)
			if (pairs[(hand[i] + -j).rank] && (int)hand[i].rank - j >= 2 && (int)hand[i].rank - j <= 14) length++;
			else if (pairs[{Rank::Ace}] && i - j == 1) length++;
			else { j--; break; };
		int j = 1;
		for (; j < 8; j++)
			if (pairs[(hand[i] + j).rank] && (int)hand[i].rank + j >= 2 && (int)hand[i].rank + j <= 14) length++;
			else { j--; break; }
		if (length >= 5)
			return { Rates::Flush, Card{Figure::None, Rank((int)hand[i].rank + j)} };
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
						return { Rates::FullHouse,  Card{ Figure::None, hand[1].rank} };
					else if (pr.first == hand[0].rank || pri.first == hand[0].rank)
						return { Rates::FullHouse,  Card{ Figure::None, hand[0].rank} };
					else
						return { Rates::FullHouse,  Card{ Figure::None, pr.first} };
	return CardValue();
}
CardValue Poker::isFOTK(const Hand& hand, const std::unordered_map<Rank, int>& map)
{
	std::vector<CardValue> result;
	for (const auto& pri : map)
		if (pri.second == 4)
			return { Rates::FOTK, Card{Figure::None, pri.first} };
	return CardValue();
}
CardValue Poker::isStraightFlush(const Hand& hand, std::unordered_map<Rank, int>& map, std::unordered_map<Card, int>& cardMap)
{
	for (int i = 0; i < 2; i++)
	{
		int length = 1;
		for (int j = 1; j < 8; j++)
			if (cardMap[{hand[i].figure, Rank((int)hand[i].rank - j)}] && (int)hand[i].rank - j >= 2 && (int)hand[i].rank - j <= 14) length++;
			else if (cardMap[{hand[i].figure, Rank::Ace}] && i - j == 1) length++;
			else { j--; break; };
		int j = 1;
		for (; j < 8; j++)
			if (cardMap[{hand[i].figure, Rank((int)hand[i].rank + j)}] && (int)hand[i].rank + j >= 2 && (int)hand[i].rank + j <= 14) length++;
			else { j--; break; }
		if (length >= 5)
			return { Rates::StraightFlush, hand[i] + j };
	}
	return CardValue();
}
CardValue Poker::isRoyalFlush(const Hand& hand, std::unordered_map<Card, int>& map)
{
	if (map[{Figure::Hearts, Rank::Ace}] && map[{Figure::Hearts, Rank::King}] && map[{Figure::Hearts, Rank::Queen}] && map[{Figure::Hearts, Rank::Jack}] && map[{Figure::Hearts, Rank::Ten}])
		return { Rates::RoyalFlush, Card(Figure::Hearts, Rank::Ace) };
	return CardValue();
}
