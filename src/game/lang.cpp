#include "lang.h"

#include "game_exception.h"
#include "game_state.h"
#include "objects.h"
#include "results.h"

#include <algorithm>
#include <iterator>
#include <numeric>
#include <vector>

const std::string S_SEP = ",";
const std::string S_SEP_SP = S_SEP + " ";

const std::string S_PLAYER_SP = "Player ";
const std::string S_GAME = "Game";

const std::string S_NONE = "???";

const std::string S_MOVE_PLAY_PYRAMID_CARD = "Play card";
const std::string S_MOVE_BUILD_PYRAMID_CARD = "Build card";
const std::string S_MOVE_DISCARD_PYRAMID_CARD = "Discard card";
const std::string S_MOVE_USE_PYRAMID_CARD_FOR_WONDER = "Build wonder";
const std::string S_MOVE_BUILD_GAME_TOKEN = "Build token";
const std::string S_MOVE_BUILD_BOX_TOKEN = "Build box token";
const std::string S_MOVE_BUILD_DISCARDED = "Build discarded";
const std::string S_MOVE_DESTROY_OBJECT_SP = "Destroy ";
const std::string S_MOVE_SELECT_WONDER = "Select Wonder";
const std::string S_MOVE_CHOOSE_START_PLAYER = "Choose start player";

const std::string S_REVEAL_GUILD = "Reveal guild";
const std::string S_REVEAL_PYRAMID_CARD = "Reveal card";
const std::string S_REVEAL_GAME_TOKEN = "Reveal token";
const std::string S_REVEAL_BOX_TOKEN = "Reveal box token";
const std::string S_REVEAL_WONDER = "Reveal wonder";
const std::string S_REVEAL_FIRST_PLAYER = "Reveal first player";

const std::string S_RESULT_DRAW = "Draw";
const std::string S_RESULT_WIN = "victory";
const std::string S_RESULT_LOSS = "defeat";

const std::string S_RESULT_CIVILIAN = "Civilian";
const std::string S_RESULT_SCIENCE = "Scientific";
const std::string S_RESULT_MILITARY = "Military";
const std::string S_RESULT_TIEBREAK = "Tiebreak";

const std::string S_AGE_SETUP = "Setup";
const std::string S_AGE_WONDER_SELECTION = "Wonder selection";
const std::string S_AGE_SP = "Age ";

const std::array<std::string, NUM_OBJECT_TYPES> S_TYPES = {{
    "brown",
    "gray",
    "blue",
    "red",
    "green",
    "yellow",
    "guild",
    "token",
    "wonder",
    "looting"
}};

std::string sanitizeName(const std::string& name)
{
    std::string sanitized = name;
    sanitized.erase(std::remove_if(sanitized.begin(), sanitized.end(), [](char c){ return !std::isalnum(c); }), sanitized.end());
    std::transform(sanitized.begin(), sanitized.end(), sanitized.begin(), [](char c){ return std::tolower(c); });
    return sanitized;
}

std::string joinActionStr(const std::vector<std::string>& tokens)
{
    if (tokens.empty()) return std::string();
 
    return std::accumulate(tokens.begin() + 1, tokens.end(), tokens[0],
        [](std::string x, std::string y) { return x + S_SEP_SP + y; }
    );
}

std::vector<std::string> splitActionStr(const std::string& actionStr)
{
    int tokenStart = 0;
    int tokenEnd = 0;
    std::vector<std::string> tokens;
    while ((tokenEnd = actionStr.find(S_SEP, tokenStart)) != (int) std::string::npos)
    {
        tokens.push_back(actionStr.substr(tokenStart, tokenEnd - tokenStart));
        tokenStart = tokenEnd + S_SEP.length();
    }
    tokens.push_back(actionStr.substr(tokenStart));
    return tokens;
}

std::string posToString(int pos)
{
    if (pos == ACT_ARG_NONE) return S_NONE;
    else return std::to_string(pos);
}

std::string objectToString(int id)
{
    if (id == OBJ_NONE) return S_NONE;
    else return objects[id].name;
}

int objectFromString(const std::string& name)
{
    std::string sanitized = sanitizeName(name);
    for (const Object& object : objects)
    {
        if (sanitized == sanitizeName(object.name)) return object.id;
    }
    throw GameException("Unknown object name.", {});
}

std::string ageToString(int age)
{
    if (age == AGE_SETUP) return S_AGE_SETUP;
    if (age == AGE_WONDER_SELECTION) return S_AGE_WONDER_SELECTION;
    return S_AGE_SP + std::to_string(age + 1);
}

std::string actorToString(int actor)
{
    if (actor == ACT_ARG_NONE) return S_NONE;
    if (actor == ACTOR_GAME) return S_GAME;
    return S_PLAYER_SP + std::to_string(actor + 1);
}

int actorFromString(const std::string& actiorStr)
{
    std::string sanitized = sanitizeName(actiorStr);
    if (sanitized == sanitizeName(S_GAME)) return ACTOR_GAME;
    for (int player = 0; player < NUM_PLAYERS; ++ player)
    {
        if (sanitized == sanitizeName(S_PLAYER_SP + std::to_string(player))) return player;
    }
    throw GameException("Unknown actor name.", {});
}

std::string actionToString(const Action& action)
{
    switch (action.type)
    {
    case ACT_MOVE_PLAY_PYRAMID_CARD:
        switch (action.arg2)
        {
        case ACT_ARG_NONE:
            return joinActionStr({S_MOVE_PLAY_PYRAMID_CARD, S_NONE});

        case ACT_ARG2_BUILD:
            return joinActionStr({S_MOVE_BUILD_PYRAMID_CARD, objectToString(action.arg1)});

        case ACT_ARG2_DISCARD:
            return joinActionStr({S_MOVE_DISCARD_PYRAMID_CARD, objectToString(action.arg1)});

        default:
            return joinActionStr({S_MOVE_USE_PYRAMID_CARD_FOR_WONDER, objectToString(action.arg1), objectToString(action.arg2)});
        }

    case ACT_MOVE_BUILD_GAME_TOKEN:
        return joinActionStr({S_MOVE_BUILD_GAME_TOKEN, objectToString(action.arg1)});

    case ACT_MOVE_BUILD_BOX_TOKEN:
        return joinActionStr({S_MOVE_BUILD_BOX_TOKEN, objectToString(action.arg1)});

    case ACT_MOVE_BUILD_DISCARDED:
        return joinActionStr({S_MOVE_BUILD_DISCARDED, objectToString(action.arg1)});

    case ACT_MOVE_DESTROY_OBJECT:
        return joinActionStr({S_MOVE_DESTROY_OBJECT_SP + S_TYPES[action.arg2], objectToString(action.arg1)});

    case ACT_MOVE_SELECT_WONDER:
        return joinActionStr({S_MOVE_SELECT_WONDER, objectToString(action.arg1)});

    case ACT_MOVE_CHOOSE_START_PLAYER:
        return joinActionStr({S_MOVE_CHOOSE_START_PLAYER, actorToString(action.arg1)});

    case ACT_REVEAL_GUILD:
        return joinActionStr({S_REVEAL_GUILD, posToString(action.arg1)});

    case ACT_REVEAL_PYRAMID_CARD:
        return joinActionStr({S_REVEAL_PYRAMID_CARD, objectToString(action.arg1), posToString(action.arg2)});

    case ACT_REVEAL_GAME_TOKEN:
        return joinActionStr({S_REVEAL_GAME_TOKEN, objectToString(action.arg1)});

    case ACT_REVEAL_BOX_TOKEN:
        return joinActionStr({S_REVEAL_BOX_TOKEN, objectToString(action.arg1)});

    case ACT_REVEAL_WONDER:
        return joinActionStr({S_REVEAL_WONDER, objectToString(action.arg1)});

    case ACT_REVEAL_FIRST_PLAYER:
        return joinActionStr({S_REVEAL_FIRST_PLAYER, actorToString(action.arg1)});

    default:
        throw GameException("Unknown action type.", {{"actionType", action.type}});
    }
}

Action actionFromString(const std::string& actionStr)
{
    std::vector<std::string> tokens = splitActionStr(actionStr);

    if (tokens.empty())
        throw GameException("Too few tokens in action string.", {});

    std::string name = sanitizeName(tokens[0]);

    if (name == sanitizeName(S_MOVE_BUILD_PYRAMID_CARD))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        return Action(ACT_MOVE_PLAY_PYRAMID_CARD, objectFromString(tokens[1]), ACT_ARG2_BUILD);
    }
    if (name == sanitizeName(S_MOVE_DISCARD_PYRAMID_CARD))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        return Action(ACT_MOVE_PLAY_PYRAMID_CARD, objectFromString(tokens[1]), ACT_ARG2_DISCARD);
    }
    if (name == sanitizeName(S_MOVE_USE_PYRAMID_CARD_FOR_WONDER))
    {
        if (tokens.size() != 3)
            throw GameException("Incorrect number of tokens in action string.", {});
        return Action(ACT_MOVE_PLAY_PYRAMID_CARD, objectFromString(tokens[1]), objectFromString(tokens[2]));
    }
    if (name == sanitizeName(S_MOVE_BUILD_GAME_TOKEN))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        return Action(ACT_MOVE_BUILD_GAME_TOKEN, objectFromString(tokens[1]));
    }
    if (name == sanitizeName(S_MOVE_BUILD_BOX_TOKEN))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        return Action(ACT_MOVE_BUILD_BOX_TOKEN, objectFromString(tokens[1]));
    }
    if (name == sanitizeName(S_MOVE_BUILD_DISCARDED))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        return Action(ACT_MOVE_BUILD_DISCARDED, objectFromString(tokens[1]));
    }
    for (int type = 0; type < NUM_OBJECT_TYPES; ++type)
    {
        if (name == sanitizeName(S_MOVE_DESTROY_OBJECT_SP + S_TYPES[type]))
        {
            if (tokens.size() != 2)
                throw GameException("Incorrect number of tokens in action string.", {});
            return Action(ACT_MOVE_DESTROY_OBJECT, objectFromString(tokens[1]), type);
        }
    }
    if (name == sanitizeName(S_MOVE_SELECT_WONDER))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        return Action(ACT_MOVE_SELECT_WONDER, objectFromString(tokens[1]));
    }
    if (name == sanitizeName(S_MOVE_CHOOSE_START_PLAYER))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        return Action(ACT_MOVE_CHOOSE_START_PLAYER, actorFromString(tokens[1]));
    }
    if (name == sanitizeName(S_REVEAL_GUILD))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        return Action(ACT_REVEAL_GUILD, std::stoi(tokens[1]));
    }
    if (name == sanitizeName(S_REVEAL_PYRAMID_CARD))
    {
        if (tokens.size() != 3)
            throw GameException("Incorrect number of tokens in action string.", {});
        return Action(ACT_REVEAL_PYRAMID_CARD, objectFromString(tokens[1]), std::stoi(tokens[2]));
    }
    if (name == sanitizeName(S_REVEAL_GAME_TOKEN))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        return Action(ACT_REVEAL_GAME_TOKEN, objectFromString(tokens[1]));
    }
    if (name == sanitizeName(S_REVEAL_BOX_TOKEN))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        return Action(ACT_REVEAL_BOX_TOKEN, objectFromString(tokens[1]));
    }
    if (name == sanitizeName(S_REVEAL_WONDER))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        return Action(ACT_REVEAL_WONDER, objectFromString(tokens[1]));
    }
    if (name == sanitizeName(S_REVEAL_FIRST_PLAYER))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        return Action(ACT_REVEAL_FIRST_PLAYER, actorFromString(tokens[1]));
    }

    throw GameException("Unknown action name.", {});
}

std::string resultToString(int res)
{
    std::string resStr;

    switch (std::abs(res))
    {
    case RESULT_DRAW:
        resStr = S_RESULT_DRAW;
        break;

    case RESULT_WIN_CIVILIAN:
        resStr = S_RESULT_CIVILIAN;
        break;

    case RESULT_WIN_SCIENCE:
        resStr = S_RESULT_SCIENCE;
        break;

    case RESULT_WIN_MILITARY:
        resStr = S_RESULT_MILITARY;
        break;

    case RESULT_WIN_TIEBREAK:
        resStr = S_RESULT_TIEBREAK;
        break;

    default:
        throw GameException("Unknown result.", {{"result", res}});
    }

    if (res > 0) resStr += " " + S_RESULT_WIN;
    else if (res < 0) resStr += " " + S_RESULT_LOSS;

    return resStr;
}
