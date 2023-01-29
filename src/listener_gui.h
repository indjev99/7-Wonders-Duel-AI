#pragma once

#include "listener.h"

#define GLFW_INCLUDE_GLEXT
#define IMGUI_DEFINE_MATH_OPERATORS

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

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
        double border;
        double rounding;
    };

    ListenerGUI();
    ~ListenerGUI();

    void notifyStart() override;
    void notifyAction(const Action& action) override;
    void notifyEnd() override;

private:

    bool closed;
    GLFWwindow* window;

    std::array<bool, NUM_DECKS> isDeckCached;
    std::array<SlotRowCol, NUM_OBJECTS> cachedRowCols;
    std::array<int, NUM_OBJECTS> wonderBuiltWithDeck;

    std::array<GLuint, NUM_OBJECTS> objectTextures;

    int pressedId;

    void onClose();

    void drawObject(int objId, const ListenerGUI::SlotRowCol& rowCol, const ListenerGUI::SpaceConfig& spaceConfig, const ImVec2& offset, int deck = DECK_NONE, const std::string& extra = "");
    void drawDeck(int deck, const ListenerGUI::SpaceConfig& spaceConfig, const SlotRowCol* rowsCols, const ImVec2& offset, int maxSizeForCache);
    void drawBuilt(int player);
    void drawDiscarded();
    void drawLooting(int from, int to, int deck);
    void drawMilitaryLead();
    void drawPyramid();
    void drawState(bool canAdvance = true);
};
