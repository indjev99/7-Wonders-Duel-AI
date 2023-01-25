#include "objects.h"

#include "constants.h"
#include "game_exception.h"
#include "player_state.h"

template <int RES, int N>
void effResource(PlayerState& state)
{
    state.resources[RES] += N;
}

template <int RES, int N>
void revEffResource(PlayerState& state)
{
    state.resources[RES] -= N;
}

template <int N>
void effMilirary(PlayerState& state)
{
    state.military += N;
}

template <int SCI>
void effScience(PlayerState& state)
{
    if (state.sciences[SCI])
    {
        state.shouldBuildGameToken = true;
    }
    else
    {
        state.sciences[SCI] = true;
        state.distincSciences++;
    }
}

#define OT_UNIT -1
#define OT_BROWN_AND_GRAY -2
#define OT_COIN_PACKETS -3

template <int TYPE>
int getCount(const PlayerState& state)
{
    if constexpr (TYPE == OT_UNIT)
    {
        return 1;
    }
    else if constexpr (TYPE == OT_BROWN_AND_GRAY)
    {
        return state.typeCounts[OT_BROWN] + state.typeCounts[OT_GRAY];
    }
    else if constexpr (TYPE == OT_COIN_PACKETS)
    {
        return state.coins / COIN_PACKET_SIZE;
    }
    else
    {
        return state.typeCounts[TYPE];
    }
}

template <int TYPE>
int getMaxCount(const PlayerState& state)
{
    return std::max(getCount<TYPE>(state), getCount<TYPE>(*state.otherPlayer));
}

template <int N, int TYPE = OT_UNIT, bool MAX = false>
void effCoins(PlayerState& state)
{
    if constexpr (MAX)
    {
        state.coins += N * getMaxCount<TYPE>(state);
    }
    else
    {
        state.coins += N * getCount<TYPE>(state);
    }
}

template <int N, int TYPE = OT_UNIT, bool MAX = false>
int scorePer(const PlayerState& state)
{
    if constexpr (MAX)
    {
        return N * getMaxCount<TYPE>(state);
    }
    else
    {
        return N * getCount<TYPE>(state);
    }
}

#define R_GLASS_AND_PAPER -1

template <int RES>
void effMarket(PlayerState& state)
{
    if constexpr (RES == R_GLASS_AND_PAPER)
    {
        effMarket<R_GLASS>(state);
        effMarket<R_PAPER>(state);
    }
    else
    {
        state.markets[RES] = true;
    }
}

void effBrownWildcard(PlayerState& state)
{
    state.brownWildcards += 1;
}

void effGrayWildcard(PlayerState& state)
{
    state.grayWildcards += 1;
}

template <int N>
void effLooting(PlayerState& state)
{
    state.otherPlayer->coins = std::max(state.otherPlayer->coins - N, 0);
}

void effPlayAgain(PlayerState& state)
{
    state.shouldPlayAgain = true;
}

void effTheAppianWay(PlayerState& state)
{
    effCoins<3>(state);
    effLooting<3>(state);
    effPlayAgain(state);
}

void effCircusMaximus(PlayerState& state)
{
    state.shouldDestroyType = OT_GRAY;
    effMilirary<1>(state);
}

void effTheColossus(PlayerState& state)
{
    effMilirary<2>(state);
}

void effTheGreatLibrary(PlayerState& state)
{
    state.shouldBuildBoxToken = true;
}

void effTheGreatLighthouse(PlayerState& state)
{
    effBrownWildcard(state);
}

void effTheHangingGardens(PlayerState& state)
{
    effCoins<6>(state);
    effPlayAgain(state);
}

void effTheMausoleum(PlayerState& state)
{
    state.shouldBuildDiscarded = true;
}

void effPiraeus(PlayerState& state)
{
    effGrayWildcard(state);
    effPlayAgain(state);
}

void effThePyramids(PlayerState& state)
{}

void effTheSphinx(PlayerState& state)
{
    effPlayAgain(state);
}

void effTheStatueOfZeus(PlayerState& state)
{
    state.shouldDestroyType = OT_BROWN;
    effMilirary<1>(state);
}

void effTheTempleOfArtemis(PlayerState& state)
{
    effCoins<12>(state);
    effPlayAgain(state);
}

std::array<Object, NUM_OBJECTS> initObjects()
{
    std::array<Object, NUM_OBJECTS> objects;

    objects[O_BROWN_LUMBER_YARD] = Object(O_BROWN_LUMBER_YARD, "Lumber Yard", OT_BROWN, Cost(), effResource<R_WOOD, 1>, revEffResource<R_WOOD, 1>);
    objects[O_BROWN_LOGGING_CAMP] = Object(O_BROWN_LOGGING_CAMP, "Logging Camp", OT_BROWN, Cost(1), effResource<R_WOOD, 1>, revEffResource<R_WOOD, 1>);
    objects[O_BROWN_CLAY_POOL] = Object(O_BROWN_CLAY_POOL, "Clay Pool", OT_BROWN, Cost(), effResource<R_CLAY, 1>, revEffResource<R_CLAY, 1>);
    objects[O_BROWN_CLAY_PIT] = Object(O_BROWN_CLAY_PIT, "Clay Pit", OT_BROWN, Cost(1), effResource<R_CLAY, 1>, revEffResource<R_CLAY, 1>);
    objects[O_BROWN_QUARRY] = Object(O_BROWN_QUARRY, "Quarry", OT_BROWN, Cost(), effResource<R_STONE, 1>, revEffResource<R_STONE, 1>);
    objects[O_BROWN_STONE_PIT] = Object(O_BROWN_STONE_PIT, "Stone Pit", OT_BROWN, Cost(1), effResource<R_STONE, 1>, revEffResource<R_STONE, 1>);
    objects[O_BROWN_SAWMILL] = Object(O_BROWN_SAWMILL, "Sawmill", OT_BROWN, Cost(2), effResource<R_WOOD, 2>, revEffResource<R_WOOD, 2>);
    objects[O_BROWN_BRICKYARD] = Object(O_BROWN_BRICKYARD, "Brickyard", OT_BROWN, Cost(2), effResource<R_CLAY, 2>, revEffResource<R_CLAY, 2>);
    objects[O_BROWN_SHELF_QUARRY] = Object(O_BROWN_SHELF_QUARRY, "Shelf Quarry", OT_BROWN, Cost(), effResource<R_STONE, 2>, revEffResource<R_STONE, 2>);
    objects[O_GRAY_GLASSWORKS] = Object(O_GRAY_GLASSWORKS, "Glassworks", OT_GRAY, Cost(1), effResource<R_GLASS, 1>, revEffResource<R_GLASS, 1>);
    objects[O_GRAY_PRESS] = Object(O_GRAY_PRESS, "Press", OT_GRAY, Cost(1), effResource<R_PAPER, 1>, revEffResource<R_PAPER, 1>);
    objects[O_GRAY_GLASSBLOWER] = Object(O_GRAY_GLASSBLOWER, "Glassblower", OT_GRAY, Cost(), effResource<R_GLASS, 1>, revEffResource<R_GLASS, 1>);
    objects[O_GRAY_DRYING_ROOM] = Object(O_GRAY_DRYING_ROOM, "Drying Room", OT_GRAY, Cost(), effResource<R_PAPER, 1>, revEffResource<R_PAPER, 1>);

    objects[O_BLUE_THEATER] = Object(O_BLUE_THEATER, "Theater", OT_BLUE, Cost(), 3);
    objects[O_BLUE_ALTAR] = Object(O_BLUE_ALTAR, "Altar", OT_BLUE, Cost(), 3);
    objects[O_BLUE_BATHS] = Object(O_BLUE_BATHS, "Baths", OT_BLUE, Cost({R_STONE}), 3);
    objects[O_BLUE_STATUE] = Object(O_BLUE_STATUE, "Statue", OT_BLUE, Cost({R_CLAY, R_CLAY}, O_BLUE_THEATER), 4);
    objects[O_BLUE_TEMPLE] = Object(O_BLUE_TEMPLE, "Temple", OT_BLUE, Cost({R_WOOD, R_PAPER}, O_BLUE_ALTAR), 4);
    objects[O_BLUE_AQUEDUCT] = Object(O_BLUE_AQUEDUCT, "Aqueduct", OT_BLUE, Cost({R_STONE, R_STONE, R_STONE}, O_BLUE_STATUE), 5);
    objects[O_BLUE_ROSTRUM] = Object(O_BLUE_ROSTRUM, "Rostrum", OT_BLUE, Cost({R_STONE, R_WOOD}), 4);
    objects[O_BLUE_TRIBUNAL] = Object(O_BLUE_TRIBUNAL, "Tribunal", OT_BLUE, Cost({R_WOOD, R_WOOD, R_GLASS}), 5);
    objects[O_BLUE_GARDENS] = Object(O_BLUE_GARDENS, "Gardens", OT_BLUE, Cost({R_CLAY, R_CLAY, R_WOOD, R_WOOD}), 6);
    objects[O_BLUE_PANTHEON] = Object(O_BLUE_PANTHEON, "Pantheon", OT_BLUE, Cost({R_CLAY, R_WOOD, R_PAPER, R_PAPER}), 6);
    objects[O_BLUE_SENATE] = Object(O_BLUE_SENATE, "Senate", OT_BLUE, Cost({R_CLAY, R_CLAY, R_STONE, R_PAPER}), 5);
    objects[O_BLUE_PALACE] = Object(O_BLUE_PALACE, "Palace", OT_BLUE, Cost({R_CLAY, R_STONE, R_WOOD, R_GLASS, R_GLASS}), 7);
    objects[O_BLUE_TOWN_HALL] = Object(O_BLUE_TOWN_HALL, "Town Hall", OT_BLUE, Cost({R_STONE, R_STONE, R_STONE, R_WOOD, R_WOOD}), 7);
    objects[O_BLUE_OBELISK] = Object(O_BLUE_OBELISK, "Obelisk", OT_BLUE, Cost({R_STONE, R_STONE, R_GLASS}), 5);

    objects[O_RED_STABLE] = Object(O_RED_STABLE, "Stable", OT_RED, Cost({R_WOOD}), effMilirary<1>);
    objects[O_RED_GARRISON] = Object(O_RED_GARRISON, "Garrison", OT_RED, Cost({R_CLAY}), effMilirary<1>);
    objects[O_RED_PALISADE] = Object(O_RED_PALISADE, "Palisade", OT_RED, Cost(2), effMilirary<1>);
    objects[O_RED_GUARD_TOWER] = Object(O_RED_GUARD_TOWER, "Guard Tower", OT_RED, Cost(), effMilirary<1>);
    objects[O_RED_HORSE_BREEDERS] = Object(O_RED_HORSE_BREEDERS, "Horse Breeders", OT_RED, Cost({R_CLAY, R_WOOD}, O_RED_STABLE), effMilirary<1>);
    objects[O_RED_BARRACKS] = Object(O_RED_BARRACKS, "Barracks", OT_RED, Cost(3, O_RED_GARRISON), effMilirary<1>);
    objects[O_RED_ARCHERY_RANGE] = Object(O_RED_ARCHERY_RANGE, "Archery Range", OT_RED, Cost({R_STONE, R_WOOD, R_PAPER}), effMilirary<2>);
    objects[O_RED_PARADE_GROUND] = Object(O_RED_PARADE_GROUND, "Parade Ground", OT_RED, Cost({R_CLAY, R_CLAY, R_GLASS}), effMilirary<2>);
    objects[O_RED_WALLS] = Object(O_RED_WALLS, "Walls", OT_RED, Cost({R_STONE, R_STONE}), effMilirary<2>);
    objects[O_RED_FORTIFICATIONS] = Object(O_RED_FORTIFICATIONS, "Fortifications", OT_RED, Cost({R_STONE, R_STONE, R_CLAY, R_PAPER}, O_RED_PALISADE), effMilirary<2>);
    objects[O_RED_SIEGE_WORKSHOP] = Object(O_RED_SIEGE_WORKSHOP, "Siege Workshop", OT_RED, Cost({R_WOOD, R_WOOD, R_WOOD, R_GLASS}, O_RED_ARCHERY_RANGE), effMilirary<2>);
    objects[O_RED_CIRCUS] = Object(O_RED_CIRCUS, "Circus", OT_RED, Cost({R_CLAY, R_CLAY, R_STONE, R_STONE}, O_RED_PARADE_GROUND), effMilirary<2>);
    objects[O_RED_ARSENAL] = Object(O_RED_ARSENAL, "Arsenal", OT_RED, Cost({R_CLAY, R_CLAY, R_CLAY, R_WOOD, R_WOOD}), effMilirary<3>);
    objects[O_RED_COURTHOUSE] = Object(O_RED_COURTHOUSE, "Corthouse", OT_RED, Cost(8), effMilirary<3>);

    objects[O_GREEN_SCRIPTORIUM] = Object(O_GREEN_SCRIPTORIUM, "Scriptrium", OT_GREEN, Cost(2), 0, effScience<S_QUILL>);
    objects[O_GREEN_PHARMACIST] = Object(O_GREEN_PHARMACIST, "Pharmacist", OT_GREEN, Cost(2), 0, effScience<S_MORTAR>);
    objects[O_GREEN_WORKSHOP] = Object(O_GREEN_WORKSHOP, "Workshop", OT_GREEN, Cost({R_PAPER}), 1, effScience<S_PENDULUM>);
    objects[O_GREEN_APOTHECARY] = Object(O_GREEN_APOTHECARY, "Apothecary", OT_GREEN, Cost({R_GLASS}), 1, effScience<S_WHEEL>);
    objects[O_GREEN_LIBRARY] = Object(O_GREEN_LIBRARY, "Library", OT_GREEN, Cost({R_STONE, R_WOOD, R_GLASS}, O_GREEN_SCRIPTORIUM), 2, effScience<S_QUILL>);
    objects[O_GREEN_DISPENSARY] = Object(O_GREEN_DISPENSARY, "Dispensary", OT_GREEN, Cost({R_CLAY, R_CLAY, R_STONE}, O_GREEN_PHARMACIST), 2, effScience<S_MORTAR>);
    objects[O_GREEN_SCHOOL] = Object(O_GREEN_SCHOOL, "School", OT_GREEN, Cost({R_WOOD, R_PAPER, R_PAPER}), 1, effScience<S_PENDULUM>);
    objects[O_GREEN_LABORATORY] = Object(O_GREEN_LABORATORY, "Laboratory", OT_GREEN, Cost({R_WOOD, R_GLASS, R_GLASS}), 1, effScience<S_WHEEL>);
    objects[O_GREEN_UNIVERSITY] = Object(O_GREEN_UNIVERSITY, "University", OT_GREEN, Cost({R_CLAY, R_GLASS, R_PAPER}, O_GREEN_SCHOOL), 2, effScience<S_GYROSCOPE>);
    objects[O_GREEN_OBSERVATORY] = Object(O_GREEN_OBSERVATORY, "Observatory", OT_GREEN, Cost({R_STONE, R_PAPER, R_PAPER}), 2, effScience<S_GYROSCOPE>);
    objects[O_GREEN_ACADEMY] = Object(O_GREEN_ACADEMY, "Academy", OT_GREEN, Cost({R_STONE, R_WOOD, R_GLASS, R_GLASS}), 3, effScience<S_SUNDIAL>);
    objects[O_GREEN_STUDY] = Object(O_GREEN_STUDY, "Study", OT_GREEN, Cost({R_WOOD, R_WOOD, R_GLASS, R_PAPER}), 3, effScience<S_SUNDIAL>);

    objects[O_YELLOW_TAVERN] = Object(O_YELLOW_TAVERN, "Tavern", OT_YELLOW, Cost(), effCoins<4>);
    objects[O_YELLOW_STONE_RESERVE] = Object(O_YELLOW_STONE_RESERVE, "Stone Reserve", OT_YELLOW, Cost(3), effMarket<R_STONE>);
    objects[O_YELLOW_CLAY_RESERVE] = Object(O_YELLOW_CLAY_RESERVE, "Clay Reserve", OT_YELLOW, Cost(3), effMarket<R_CLAY>);
    objects[O_YELLOW_WOOD_RESERVE] = Object(O_YELLOW_WOOD_RESERVE, "Wood Reserve", OT_YELLOW, Cost(3), effMarket<R_WOOD>);
    objects[O_YELLOW_BREWERY] = Object(O_YELLOW_BREWERY, "Brewery", OT_YELLOW, Cost(), effCoins<6>);
    objects[O_YELLOW_FORUM] = Object(O_YELLOW_FORUM, "Forum", OT_YELLOW, Cost(3, {R_CLAY}), effGrayWildcard);
    objects[O_YELLOW_CARAVANSERY] = Object(O_YELLOW_CARAVANSERY, "Caravansery", OT_YELLOW, Cost(2, {R_GLASS, R_PAPER}), effBrownWildcard);
    objects[O_YELLOW_CUSOMS_HOUSE] = Object(O_YELLOW_CUSOMS_HOUSE, "Customs House", OT_YELLOW, Cost(4), effMarket<R_GLASS_AND_PAPER>);
    objects[O_YELLOW_LIGHTHOUSE] = Object(O_YELLOW_LIGHTHOUSE, "Lighthouse", OT_YELLOW, Cost({R_CLAY, R_CLAY, R_GLASS}, O_YELLOW_TAVERN), 3, effCoins<1, OT_YELLOW>);
    objects[O_YELLOW_ARENA] = Object(O_YELLOW_ARENA, "Arena", OT_YELLOW, Cost({R_CLAY, R_STONE, R_WOOD}, O_YELLOW_BREWERY), 3, effCoins<2, OT_WONDER>);
    objects[O_YELLOW_CHAMBER_OF_COMMERCE] = Object(O_YELLOW_CHAMBER_OF_COMMERCE, "Chamber of Commerce", OT_YELLOW, Cost({R_PAPER, R_PAPER}), 3, effCoins<3, OT_GRAY>);
    objects[O_YELLOW_PORT] = Object(O_YELLOW_PORT, "Port", OT_YELLOW, Cost({R_WOOD, R_GLASS, R_PAPER}), 3, effCoins<2, OT_BROWN>);
    objects[O_YELLOW_ARMORY] = Object(O_YELLOW_ARMORY, "Armory", OT_YELLOW, Cost({R_STONE, R_STONE, R_GLASS}), 3, effCoins<1, OT_RED>);

    objects[O_GUILD_MERCHANTS_GUILD] = Object(O_GUILD_MERCHANTS_GUILD, "Merchants Guild", OT_GUILD, Cost({R_CLAY, R_WOOD, R_GLASS, R_PAPER}), scorePer<1, OT_YELLOW, true>, effCoins<1, OT_YELLOW, true>);
    objects[O_GUILD_SHIPOWNERS_GUILD] = Object(O_GUILD_SHIPOWNERS_GUILD, "Shipowners Guild", OT_GUILD, Cost({R_CLAY, R_STONE, R_GLASS, R_PAPER}), scorePer<1, OT_BROWN_AND_GRAY, true>, effCoins<1, OT_BROWN_AND_GRAY, true>);
    objects[O_GUILD_BUILDERS_GUILD] = Object(O_GUILD_BUILDERS_GUILD, "Builders Guild", OT_GUILD, Cost({R_STONE, R_STONE, R_CLAY, R_WOOD, R_GLASS}), scorePer<2, OT_WONDER, true>);
    objects[O_GUILD_MAGISTRATES_GUILD] = Object(O_GUILD_MAGISTRATES_GUILD, "Magistrates Guild", OT_GUILD, Cost({R_WOOD, R_WOOD, R_CLAY, R_PAPER}), scorePer<1, OT_BLUE, true>, effCoins<1, OT_BLUE, true>);
    objects[O_GUILD_SCIENTISTS_GUILD] = Object(O_GUILD_SCIENTISTS_GUILD, "Scientists Guild", OT_GUILD, Cost({R_CLAY, R_CLAY, R_WOOD, R_WOOD}), scorePer<1, OT_GREEN, true>, effCoins<1, OT_GREEN, true>);
    objects[O_GUILD_MONEYLENDERS_GUILD] = Object(O_GUILD_MONEYLENDERS_GUILD, "Moneylenderers Guild", OT_GUILD, Cost({R_STONE, R_STONE, R_WOOD, R_WOOD}), scorePer<1, OT_COIN_PACKETS, true>);
    objects[O_GUILD_TACTICIANS_GUILD] = Object(O_GUILD_TACTICIANS_GUILD, "Tacticians Guild", OT_GUILD, Cost({R_STONE, R_STONE, R_CLAY, R_PAPER}), scorePer<1, OT_RED, true>, effCoins<1, OT_RED, true>);

    objects[O_TOKEN_AGRICULTURE] = Object(O_TOKEN_AGRICULTURE, "Agriculture", OT_TOKEN, Cost(), 4, effCoins<4>);
    objects[O_TOKEN_ARCHITECTURE] = Object(O_TOKEN_ARCHITECTURE, "Architecture", OT_TOKEN, Cost());
    objects[O_TOKEN_ECONOMY] = Object(O_TOKEN_ECONOMY, "Economy", OT_TOKEN, Cost());
    objects[O_TOKEN_LAW] = Object(O_TOKEN_LAW, "Law", OT_TOKEN, Cost(), effScience<S_LAW>);
    objects[O_TOKEN_MASONRY] = Object(O_TOKEN_MASONRY, "Masonry", OT_TOKEN, Cost());
    objects[O_TOKEN_MATHEMATICS] = Object(O_TOKEN_MATHEMATICS, "Mathematics", OT_TOKEN, Cost(), scorePer<3, OT_TOKEN>);
    objects[O_TOKEN_PHILOSOPHY] = Object(O_TOKEN_PHILOSOPHY, "Philosophy", OT_TOKEN, Cost(), 7);
    objects[O_TOKEN_STRATEGY] = Object(O_TOKEN_STRATEGY, "Strategy", OT_TOKEN, Cost());
    objects[O_TOKEN_THEOLOGY] = Object(O_TOKEN_THEOLOGY, "Theology", OT_TOKEN, Cost());
    objects[O_TOKEN_URBANISM] = Object(O_TOKEN_URBANISM, "Urbanism", OT_TOKEN, Cost(), effCoins<6>);

    objects[O_WONDER_THE_APPIAN_WAY] = Object(O_WONDER_THE_APPIAN_WAY, "The Appian Way", OT_WONDER, Cost({R_STONE, R_STONE, R_CLAY, R_CLAY, R_PAPER}), effTheAppianWay);
    objects[O_WONDER_CIRCUS_MAXIMUS] = Object(O_WONDER_CIRCUS_MAXIMUS, "Circus Maximus", OT_WONDER, Cost({R_STONE, R_STONE, R_WOOD, R_GLASS}), effCircusMaximus);
    objects[O_WONDER_THE_COLOSSUS] = Object(O_WONDER_THE_COLOSSUS, "The Colossus", OT_WONDER, Cost({R_CLAY, R_CLAY, R_CLAY, R_GLASS}), effTheColossus);
    objects[O_WONDER_THE_GREAT_LIBRARY] = Object(O_WONDER_THE_GREAT_LIBRARY, "The Great Library", OT_WONDER, Cost({R_WOOD, R_WOOD, R_WOOD, R_GLASS, R_PAPER}), effTheGreatLibrary);
    objects[O_WONDER_THE_GREAT_LIGHTHOUSE] = Object(O_WONDER_THE_GREAT_LIGHTHOUSE, "The Great Lighthouse", OT_WONDER, Cost({R_WOOD, R_STONE, R_PAPER, R_PAPER}), effTheGreatLighthouse);
    objects[O_WONDER_THE_HANGING_GARDENS] = Object(O_WONDER_THE_HANGING_GARDENS, "The Hanging Gardens", OT_WONDER, Cost({R_WOOD, R_WOOD, R_GLASS, R_PAPER}), effTheHangingGardens);
    objects[O_WONDER_THE_MAUSOLEUM] = Object(O_WONDER_THE_MAUSOLEUM, "The Mausoleum", OT_WONDER, Cost({R_CLAY, R_CLAY, R_GLASS, R_GLASS, R_PAPER}), effTheMausoleum);
    objects[O_WONDER_PIRAEUS] = Object(O_WONDER_PIRAEUS, "Piraeus", OT_WONDER, Cost({R_WOOD, R_WOOD, R_STONE, R_CLAY}), effPiraeus);
    objects[O_WONDER_THE_PYRAMIDS] = Object(O_WONDER_THE_PYRAMIDS, "The Pyramids", OT_WONDER, Cost({R_STONE, R_STONE, R_STONE, R_PAPER}), effThePyramids);
    objects[O_WONDER_THE_SPHINX] = Object(O_WONDER_THE_SPHINX, "The Sphinx", OT_WONDER, Cost({R_STONE, R_CLAY, R_GLASS, R_GLASS}), effTheSphinx);
    objects[O_WONDER_THE_STATUE_OF_ZEUS] = Object(O_WONDER_THE_STATUE_OF_ZEUS, "The Statue of Zeus", OT_WONDER, Cost({R_STONE, R_WOOD, R_CLAY, R_PAPER, R_PAPER}), effTheStatueOfZeus);
    objects[O_WONDER_THE_TEMPLE_OF_ARTEMIS] = Object(O_WONDER_THE_TEMPLE_OF_ARTEMIS, "The Temple of Artemis", OT_WONDER, Cost({R_WOOD, R_STONE, R_GLASS, R_PAPER}), effTheTempleOfArtemis);

    objects[O_LOOTING_LOOTING_1] = Object(O_LOOTING_LOOTING_1, "Looting 1", OT_LOOTING, Cost(), effLooting<2>);
    objects[O_LOOTING_LOOTING_2] = Object(O_LOOTING_LOOTING_2, "Looting 2", OT_LOOTING, Cost(), effLooting<5>);

    for (int id = 0; id < NUM_OBJECTS; ++id)
    {
        if (objects[id].id != id)
            throw GameException("Object id does not match its index.", {{"objectIndex", id}, {"objectId", objects[id].id}});
    }

    return objects;
}

std::array<int, NUM_OBJECT_TYPES + 1> initObjectTypeStarts()
{
    std::array<int, NUM_OBJECT_TYPES + 1> objectTypeStarts;

    int currType = -1;
    for (int id = 0; id < NUM_OBJECTS; ++id)
    {
        if (objects[id].type < currType)
            throw GameException("Objects not sorted by type.", {{"objectId", id}, {"objectType", objects[id].type}, {"currType", currType}});

        while (currType < objects[id].type)
        {
            currType++;
            objectTypeStarts[currType] = id;
        }
    }

    while (currType < NUM_OBJECT_TYPES)
    {
        currType++;
        objectTypeStarts[currType] = NUM_OBJECTS;
    }

    return objectTypeStarts;
}

const std::array<Object, NUM_OBJECTS> objects = initObjects();

const std::array<int, NUM_OBJECT_TYPES + 1> objectTypeStarts = initObjectTypeStarts();