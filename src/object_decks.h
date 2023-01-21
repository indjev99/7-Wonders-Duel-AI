#pragma once

#include "objects.h"

#include <array>

#define NUM_AGE_1_CARDS 23
#define NUM_AGE_2_CARDS 23
#define NUM_AGE_3_CARDS 20
#define NUM_GUILD_CARDS 7
#define NUM_TOKENS 10
#define NUM_WONDERS 12
#define NUM_LOOTINGS 2

extern const std::array<int, NUM_AGE_1_CARDS> age1Cards;
extern const std::array<int, NUM_AGE_2_CARDS> age2Cards;
extern const std::array<int, NUM_AGE_3_CARDS> age3Cards;
extern const std::array<int, NUM_GUILD_CARDS> guildCards;
extern const std::array<int, NUM_TOKENS> tokens;
extern const std::array<int, NUM_WONDERS> wonders;
extern const std::array<int, NUM_LOOTINGS> lootings;
