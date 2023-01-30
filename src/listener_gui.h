#pragma once

#include "listener.h"

#define GLFW_INCLUDE_GLEXT
#define IMGUI_DEFINE_MATH_OPERATORS

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

struct PlayerGUI;

struct ListenerGUI final : Listener
{
    struct SlotRowCol
    {
        int row;
        int col;
    };

    struct SpaceConfig
    {
        ImVec2 size;
        ImVec2 sizegap;
    };

    ListenerGUI();
    ~ListenerGUI();

    void notifyStart() override;
    void notifyActionPre(const Action& action) override;
    void notifyActionPost(const Action& action) override;
    void notifyEnd() override;

private:

    bool closed;
    GLFWwindow* window;

    std::array<bool, NUM_DECKS> isDeckCached;
    std::array<SlotRowCol, NUM_OBJECTS> cachedRowCols;
    std::array<int, NUM_OBJECTS> wonderBuiltWithDeck;

    int pressedId;
    bool lastMoveWasFromGui;

    void onClose();

    static constexpr int NUM_COPTS = 3;
    static constexpr int COPT_BUILD = 0;
    static constexpr int COPT_DISCARD = 1;
    static constexpr int COPT_WONDER = 2;

    static constexpr int NUM_TEXTURES = NUM_OBJECTS + NUM_DECKS * 2 + NUM_PLAYERS * 2 + NUM_COPTS + 5;
    static constexpr int O_TEXTURE_DECKS = NUM_OBJECTS;
    static constexpr int O_TEXTURE_DECKS_ROTATED = O_TEXTURE_DECKS + NUM_DECKS;
    static constexpr int O_TEXTURE_PLAYER_ARROWS = O_TEXTURE_DECKS_ROTATED + NUM_DECKS;
    static constexpr int O_TEXTURE_PLAYER_BUTTONS = O_TEXTURE_PLAYER_ARROWS + NUM_PLAYERS;
    static constexpr int O_TEXTURE_COPT_BUTTONS = O_TEXTURE_PLAYER_BUTTONS + NUM_PLAYERS;
    static constexpr int O_TEXTURE_ADVANCE_BUTTON = O_TEXTURE_COPT_BUTTONS + NUM_COPTS;
    static constexpr int O_TEXTURE_SIDE_BOARD = O_TEXTURE_ADVANCE_BUTTON + 1;
    static constexpr int O_TEXTURE_MILITARY_LEAD = O_TEXTURE_SIDE_BOARD + 1;
    static constexpr int O_TEXTURE_COINS = O_TEXTURE_MILITARY_LEAD + 1;
    static constexpr int O_TEXTURE_SCORE = O_TEXTURE_COINS + 1;

    static constexpr GLint TEXTURE_NONE = -1;

    static constexpr int NUM_FONTS = 2;
    static constexpr int MAIN_FONT = 0;
    static constexpr int SMALL_FONT = 1;

    std::array<bool, NUM_TEXTURES> isHighlighted;

    std::array<GLuint, NUM_TEXTURES> objectTextures;
    std::array<ImFont*, NUM_FONTS> fonts;

    static GLuint loadTexture(const std::string& objName);
    void loadDeckTexture(int id, const std::string& objName);

    bool drawCost(int objId, int player, const ImVec2& offset);
    bool drawObject(int objId, const ListenerGUI::SlotRowCol& rowCol, const ListenerGUI::SpaceConfig& spaceConfig, const ImVec2& offset, const std::string& text = "");
    void drawDeck(int deck, const ListenerGUI::SpaceConfig& spaceConfig, const SlotRowCol* rowsCols, const ImVec2& offset, int maxSizeForCache);
    void drawPlayerStats(int player);
    void drawBuilt(int player);
    void drawDiscarded();
    void drawMilitaryLead();
    void drawPyramid();
    void drawButtons(bool advanceButton);
    void drawState(bool advanceButton = true, bool fastAdvance = false, PlayerGUI* playerGui = nullptr);

    friend PlayerGUI;
};
