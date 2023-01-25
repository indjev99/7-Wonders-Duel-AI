#include "lang.h"

#include "game_exception.h"
#include "game_state.h"
#include "objects.h"

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
const std::string S_MOVE_SELECT_WONDER = "Select wonder";

const std::string S_REVEAL_GUILD = "Reveal guild";
const std::string S_REVEAL_PYRAMID_CARD = "Reveal card";
const std::string S_REVEAL_GAME_TOKEN = "Reveal token";
const std::string S_REVEAL_BOX_TOKEN = "Reveal box token";
const std::string S_REVEAL_WONDER = "Reveal wonder";

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
    if (pos == ACT_ARG_EMPTY) return S_NONE;
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

std::string actorToString(int actor)
{
    if (actor == ACTOR_GAME) return S_GAME;
    else return S_PLAYER_SP + std::to_string(actor);
}

std::string actionToString(const Action& action)
{
    switch (action.type)
    {
    case ACT_MOVE_PLAY_PYRAMID_CARD:
        switch (action.arg2)
        {
        case ACT_ARG_EMPTY:
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

    case ACT_MOVE_SELECT_WONDER:
        return joinActionStr({S_MOVE_SELECT_WONDER, objectToString(action.arg1)});

    case ACT_REVEAL_GUILD:
        return joinActionStr({S_REVEAL_GUILD, posToString(action.arg1)});

    case ACT_REVEAL_PYRAMID_CARD:
        return joinActionStr({S_REVEAL_PYRAMID_CARD, posToString(action.arg1), objectToString(action.arg2)});

    case ACT_REVEAL_GAME_TOKEN:
        return joinActionStr({S_REVEAL_GAME_TOKEN, objectToString(action.arg1)});

    case ACT_REVEAL_BOX_TOKEN:
        return joinActionStr({S_REVEAL_BOX_TOKEN, objectToString(action.arg1)});

    case ACT_REVEAL_WONDER:
        return joinActionStr({S_REVEAL_WONDER, objectToString(action.arg1)});

    default:
        throw GameException("Unknown action type.", {{"actionType", action.type}});
    }
}

Action actionFromString(const std::string& actionStr)
{
    Action action;

    std::vector<std::string> tokens = splitActionStr(actionStr);

    if (tokens.empty())
        throw GameException("Too few tokens in action string.", {});

    std::string name = sanitizeName(tokens[0]);

    if (name == sanitizeName(S_MOVE_BUILD_PYRAMID_CARD))
    {
        if (tokens.size() != 3)
            throw GameException("Incorrect number of tokens in action string.", {});
        action = Action(ACT_MOVE_PLAY_PYRAMID_CARD, objectFromString(tokens[1]), ACT_ARG2_BUILD);
    }
    else if (name == sanitizeName(S_MOVE_DISCARD_PYRAMID_CARD))
    {
        if (tokens.size() != 3)
            throw GameException("Incorrect number of tokens in action string.", {});
        action = Action(ACT_MOVE_PLAY_PYRAMID_CARD, objectFromString(tokens[1]), ACT_ARG2_DISCARD);
    }
    else if (name == sanitizeName(S_MOVE_USE_PYRAMID_CARD_FOR_WONDER))
    {
        if (tokens.size() != 3)
            throw GameException("Incorrect number of tokens in action string.", {});
        action = Action(ACT_MOVE_PLAY_PYRAMID_CARD, objectFromString(tokens[1]), objectFromString(tokens[2]));
    }
    else if (name == sanitizeName(S_MOVE_BUILD_GAME_TOKEN))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        action = Action(ACT_MOVE_BUILD_GAME_TOKEN, objectFromString(tokens[1]));
    }
    else if (name == sanitizeName(S_MOVE_BUILD_BOX_TOKEN))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        action = Action(ACT_MOVE_BUILD_BOX_TOKEN, objectFromString(tokens[1]));
    }
    else if (name == sanitizeName(S_MOVE_BUILD_DISCARDED))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        action = Action(ACT_MOVE_BUILD_DISCARDED, objectFromString(tokens[1]));
    }
    else if (name == sanitizeName(S_MOVE_SELECT_WONDER))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        action = Action(ACT_MOVE_SELECT_WONDER, objectFromString(tokens[1]));
    }
    else if (name == sanitizeName(S_REVEAL_GUILD))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        action = Action(ACT_REVEAL_GUILD, std::stoi(tokens[1]));
    }
    else if (name == sanitizeName(S_REVEAL_PYRAMID_CARD))
    {
        if (tokens.size() != 3)
            throw GameException("Incorrect number of tokens in action string.", {});
        action = Action(ACT_REVEAL_PYRAMID_CARD, std::stoi(tokens[1]), objectFromString(tokens[2]));
    }
    else if (name == sanitizeName(S_REVEAL_GAME_TOKEN))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        action = Action(ACT_REVEAL_GAME_TOKEN, objectFromString(tokens[1]));
    }
    else if (name == sanitizeName(S_REVEAL_BOX_TOKEN))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        action = Action(ACT_REVEAL_BOX_TOKEN, objectFromString(tokens[1]));
    }
    else if (name == sanitizeName(S_REVEAL_WONDER))
    {
        if (tokens.size() != 2)
            throw GameException("Incorrect number of tokens in action string.", {});
        action = Action(ACT_REVEAL_WONDER, objectFromString(tokens[1]));
    }
    else
        throw GameException("Unknown action name.", {});

    return action;
}
