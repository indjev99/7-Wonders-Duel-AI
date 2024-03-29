#pragma once

#include "object.h"
#include "object_types.h"

#include <array>

#define NUM_OBJECTS 97

#define O_BROWN_LUMBER_YARD 0
#define O_BROWN_LOGGING_CAMP 1
#define O_BROWN_CLAY_POOL 2
#define O_BROWN_CLAY_PIT 3
#define O_BROWN_QUARRY 4
#define O_BROWN_STONE_PIT 5
#define O_BROWN_SAWMILL 6
#define O_BROWN_BRICKYARD 7
#define O_BROWN_SHELF_QUARRY 8

#define O_GRAY_GLASSWORKS 9
#define O_GRAY_PRESS 10
#define O_GRAY_GLASSBLOWER 11
#define O_GRAY_DRYING_ROOM 12

#define O_BLUE_THEATER 13
#define O_BLUE_ALTAR 14
#define O_BLUE_BATHS 15
#define O_BLUE_STATUE 16
#define O_BLUE_TEMPLE 17
#define O_BLUE_AQUEDUCT 18
#define O_BLUE_ROSTRUM 19
#define O_BLUE_COURTHOUSE 20
#define O_BLUE_GARDENS 21
#define O_BLUE_PANTHEON 22
#define O_BLUE_SENATE 23
#define O_BLUE_PALACE 24
#define O_BLUE_TOWN_HALL 25
#define O_BLUE_OBELISK 26

#define O_RED_STABLE 27
#define O_RED_GARRISON 28
#define O_RED_PALISADE 29
#define O_RED_GUARD_TOWER 30
#define O_RED_HORSE_BREEDERS 31
#define O_RED_BARRACKS 32
#define O_RED_ARCHERY_RANGE 33
#define O_RED_PARADE_GROUND 34
#define O_RED_WALLS 35
#define O_RED_FORTIFICATIONS 36
#define O_RED_SIEGE_WORKSHOP 37
#define O_RED_CIRCUS 38
#define O_RED_ARSENAL 39
#define O_RED_PRETORIUM 40

#define O_GREEN_SCRIPTORIUM 41
#define O_GREEN_PHARMACIST 42
#define O_GREEN_WORKSHOP 43
#define O_GREEN_APOTHECARY 44
#define O_GREEN_LIBRARY 45
#define O_GREEN_DISPENSARY 46
#define O_GREEN_SCHOOL 47
#define O_GREEN_LABORATORY 48
#define O_GREEN_UNIVERSITY 49
#define O_GREEN_OBSERVATORY 50
#define O_GREEN_ACADEMY 51
#define O_GREEN_STUDY 52

#define O_YELLOW_TAVERN 53
#define O_YELLOW_STONE_RESERVE 54
#define O_YELLOW_CLAY_RESERVE 55
#define O_YELLOW_WOOD_RESERVE 56
#define O_YELLOW_BREWERY 57
#define O_YELLOW_FORUM 58
#define O_YELLOW_CARAVANSERY 59
#define O_YELLOW_CUSOMS_HOUSE 60
#define O_YELLOW_LIGHTHOUSE 61
#define O_YELLOW_ARENA 62
#define O_YELLOW_CHAMBER_OF_COMMERCE 63
#define O_YELLOW_PORT 64
#define O_YELLOW_ARMORY 65

#define O_GUILD_MERCHANTS_GUILD 66
#define O_GUILD_SHIPOWNERS_GUILD 67
#define O_GUILD_BUILDERS_GUILD 68
#define O_GUILD_MAGISTRATES_GUILD 69
#define O_GUILD_SCIENTISTS_GUILD 70
#define O_GUILD_MONEYLENDERS_GUILD 71
#define O_GUILD_TACTICIANS_GUILD 72

#define O_TOKEN_AGRICULTURE 73
#define O_TOKEN_ARCHITECTURE 74
#define O_TOKEN_ECONOMY 75
#define O_TOKEN_LAW 76
#define O_TOKEN_MASONRY 77
#define O_TOKEN_MATHEMATICS 78
#define O_TOKEN_PHILOSOPHY 79
#define O_TOKEN_STRATEGY 80
#define O_TOKEN_THEOLOGY 81
#define O_TOKEN_URBANISM 82

#define O_WONDER_THE_APPIAN_WAY 83
#define O_WONDER_CIRCUS_MAXIMUS 84
#define O_WONDER_THE_COLOSSUS 85
#define O_WONDER_THE_GREAT_LIBRARY 86
#define O_WONDER_THE_GREAT_LIGHTHOUSE 87
#define O_WONDER_THE_HANGING_GARDENS 88
#define O_WONDER_THE_MAUSOLEUM 89
#define O_WONDER_PIRAEUS 90
#define O_WONDER_THE_PYRAMIDS 91
#define O_WONDER_THE_SPHINX 92
#define O_WONDER_THE_STATUE_OF_ZEUS 93
#define O_WONDER_THE_TEMPLE_OF_ARTEMIS 94

#define O_LOOTING_LOOTING_1 95
#define O_LOOTING_LOOTING_2 96

extern const std::array<Object, NUM_OBJECTS> objects;
extern const std::array<int, NUM_OBJECT_TYPES + 1> objectTypeStarts;
