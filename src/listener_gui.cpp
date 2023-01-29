#include "listener_gui.h"

#include <iostream>

#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define NUM_CARD_DECKS 4

const std::array<std::array<ListenerGUI::SlotRowCol, PYRAMID_SIZE>, NUM_AGES> slotRowCols = {{
    {{
        {1, 4},
        {1, 6},
        {2, 3},
        {2, 5},
        {2, 7},
        {3, 2},
        {3, 4},
        {3, 6},
        {3, 8},
        {4, 1},
        {4, 3},
        {4, 5},
        {4, 7},
        {4, 9},
        {5, 0},
        {5, 2},
        {5, 4},
        {5, 6},
        {5, 8},
        {5, 10}
    }},
    {{
        {1, 0},
        {1, 2},
        {1, 4},
        {1, 6},
        {1, 8},
        {1, 10},
        {2, 1},
        {2, 3},
        {2, 5},
        {2, 7},
        {2, 9},
        {3, 2},
        {3, 4},
        {3, 6},
        {3, 8},
        {4, 3},
        {4, 5},
        {4, 7},
        {5, 4},
        {5, 6}
    }},
    {{
        {0, 4},
        {0, 6},
        {1, 3},
        {1, 5},
        {1, 7},
        {2, 2},
        {2, 4},
        {2, 6},
        {2, 8},
        {3, 3},
        {3, 7},
        {4, 2},
        {4, 4},
        {4, 6},
        {4, 8},
        {5, 3},
        {5, 5},
        {5, 7},
        {6, 4},
        {6, 6}
    }},
}};

const std::array<ListenerGUI::SlotRowCol, NUM_WONDERS_REVEALED> wonderRowCols = {{
    {0, 0},
    {0, 2},
    {1, 0},
    {1, 2}
}};

const std::array<ListenerGUI::SlotRowCol, NUM_TOKENS> gameTokenRowCols = {{
    {0, 0},
    {1, 0},
    {2, 0},
    {3, 0},
    {4, 0}
}};

const std::array<ListenerGUI::SlotRowCol, NUM_TOKENS> boxTokenRowCols = {{
    {0, 0},
    {1, 0},
    {2, 0}
}};

const std::array<ImVec4, NUM_CARD_DECKS> deckCols = {{
    ImVec4(0.74, 0.38, 0.18, 1.0),
    ImVec4(0.00, 0.57, 0.81, 1.0),
    ImVec4(0.71, 0.58, 0.76, 1.0),
    ImVec4(0.45, 0.42, 0.68, 1.0)
}};

const std::array<ImVec4, NUM_OBJECT_TYPES> typeCols = {{
    ImVec4(0.44, 0.20, 0.11, 1.0),
    ImVec4(0.58, 0.57, 0.56, 1.0),
    ImVec4(0.00, 0.46, 0.68, 1.0),
    ImVec4(0.77, 0.10, 0.12, 1.0),
    ImVec4(0.00, 0.63, 0.35, 1.0),
    ImVec4(0.90, 0.72, 0.04, 1.0),
    ImVec4(0.50, 0.37, 0.60, 1.0),
    ImVec4(0.00, 0.47, 0.27, 1.0),
    ImVec4(0.82, 0.75, 0.40, 1.0),
    ImVec4(0.00, 0.00, 0.00, 1.0)
}};

const ListenerGUI::SpaceConfig cardPyramidConfig = {
    ImVec2(44, 68),
    ImVec2(50, 40),
    1.5
};

const ListenerGUI::SpaceConfig cardBuiltConfig = {
    ImVec2(44, 68),
    ImVec2(50, 18),
    1.5
};

const ListenerGUI::SpaceConfig wonderConfig = {
    ImVec2(100, 65),
    ImVec2(110, 75),
    2.5
};

const ListenerGUI::SpaceConfig tokenConfig = {
    ImVec2(35, 35),
    ImVec2(40, 40),
    1.0
};

const ImVec2 pyramidCardOffset(10, 165);
const ImVec2 revealedWonderOffset(52, 249);
const ImVec2 gameTokenOffset(320, 221.5);
const ImVec2 boxTokenOffset(365, 261.5);

const std::array<ImVec2, NUM_PLAYERS> selectedWondersOffset = {{
    ImVec2(10, 488),
    ImVec2(10, 10)
}};

const std::array<ImVec2, NUM_PLAYERS> builtCardOffset = {{
    ImVec2(230, 488),
    ImVec2(230, 82)
}};

const std::array<ImVec2, NUM_PLAYERS> builtTokenOffset = {{
    ImVec2(584, 488),
    ImVec2(584, 115)
}};

const ImVec2 textPos = ImVec2(0.5, 0.0);

const int MAX_BUILT_CARD_COL = 5;
const int MAX_BUILT_TOKEN_COL = 3;

const double TOP_OFFSET = 15;
const double SIZE_MULT = 1.75;

const double BORDER_DARK = 0.5;

ImVec4 borderCol(const ImVec4& col)
{
    return ImVec4(col.x * BORDER_DARK, col.y * BORDER_DARK, col.z * BORDER_DARK, col.w);
}

void ListenerGUI::drawObject(int objId, const ListenerGUI::SlotRowCol& rowCol, const ListenerGUI::SpaceConfig& spaceConfig, const ImVec2& offset, int deck)
{
    bool pressed = false;

    ImVec4 col = objId != OBJ_NONE ? typeCols[objects[objId].type] : deckCols[deck];
    std::string name = objId != OBJ_NONE ? objects[objId].name : "##";

    double x = spaceConfig.sizegap.x * rowCol.col / 2 + offset.x;
    double y = spaceConfig.sizegap.y * rowCol.row + offset.y;

    ImVec4 bCol = borderCol(col);

    ImGui::PushID(rowCol.row);
    ImGui::PushID(rowCol.col);

    ImGui::PushStyleColor(ImGuiCol_Button, bCol);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bCol);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, bCol);

    ImGui::SetCursorPos(ImVec2(x * SIZE_MULT, y * SIZE_MULT + TOP_OFFSET));
    pressed = pressed || ImGui::ButtonEx(("## Border of " + name).c_str(), ImVec2(spaceConfig.size.x * SIZE_MULT, spaceConfig.size.y * SIZE_MULT), ImGuiButtonFlags_AllowItemOverlap);
    ImGui::SetItemAllowOverlap();

    ImGui::PopStyleColor(3);

    x += spaceConfig.border;
    y += spaceConfig.border;

    double sizeX = spaceConfig.size.x - 2 * spaceConfig.border;
    double sizeY = spaceConfig.size.y - 2 * spaceConfig.border;

    ImGui::PushStyleColor(ImGuiCol_Button, col);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, col);

    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, textPos);

    ImGui::SetCursorPos(ImVec2(x * SIZE_MULT, y * SIZE_MULT + TOP_OFFSET));
    pressed = pressed || ImGui::ButtonEx(name.c_str(), ImVec2(sizeX * SIZE_MULT, sizeY * SIZE_MULT), ImGuiButtonFlags_AllowItemOverlap);
    ImGui::SetItemAllowOverlap();

    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(3);

    ImGui::PopID();
    ImGui::PopID();

    if (objId != OBJ_NONE && pressed) pressedId = objId;
}

void ListenerGUI::drawDeck(int deck, const ListenerGUI::SpaceConfig& spaceConfig, const SlotRowCol* rowsCols, const ImVec2& offset, int maxSizeForCache)
{
    if (game->getDeckSize(deck) >= maxSizeForCache)
    {
        isDeckCached[deck] = true;
        for (int i = 0; i < game->getDeckSize(deck); i++)
        {
            int objectId = game->getDeckElem(deck, i);
            cachedRowCols[objectId] = rowsCols[i];
        }
    }

    ImGui::PushID(deck);

    for (int i = 0; i < game->getDeckSize(deck); i++)
    {
        int objectId = game->getDeckElem(deck, i);
        SlotRowCol rowCol = isDeckCached[deck] ? cachedRowCols[objectId] : rowsCols[i];
        drawObject(objectId, rowCol, spaceConfig, offset);
    }

    ImGui::PopID();
}

void ListenerGUI::drawPyramid()
{
    int age = game->getCurrAge();

    if (age < 0) return;

    ImGui::PushID(DECK_CARD_PYRAMID);

    for (int i = 0; age >= 0 && i < PYRAMID_SIZE; i++)
    {
        const PyramidSlot& slot = game->getPyramidSlot(i);
        if (slot.objectId == OBJ_NONE) continue;
        int id = slot.objectId != SLOT_UNREVEALED ? slot.objectId : OBJ_NONE;
        drawObject(id, slotRowCols[age][i], cardPyramidConfig, pyramidCardOffset, slot.deck);
    }

    ImGui::PopID();
}

void ListenerGUI::drawBuilt(int player)
{
    const PlayerState& state = game->getPlayerState(player);

    std::array<int, NUM_OBJECT_TYPES> cntByType;
    std::fill(cntByType.begin(), cntByType.end(), 0);

    ImGui::PushID("BUILT");
    ImGui::PushID(player);

    for (int i = 0; i < NUM_OBJECTS; ++i)
    {
        int type = objects[i].type;

        if (type == OT_LOOTING) continue;
        if (!state.objectsBuilt[i]) continue;

        int row = cntByType[type];
        cntByType[type]++;

        int maxCol = type == OT_TOKEN ? MAX_BUILT_TOKEN_COL : MAX_BUILT_CARD_COL;

        if (player == 0) row -= std::max(0, state.typeCounts[type] - maxCol);
        else row -= std::min(maxCol, state.typeCounts[type]) - 1;

        if (type == OT_TOKEN) drawObject(i, SlotRowCol{row, 0}, tokenConfig, builtTokenOffset[player]);
        else if (type == OT_WONDER) drawObject(i, cachedRowCols[i], wonderConfig, selectedWondersOffset[player]);
        else drawObject(i, SlotRowCol{row, type}, cardBuiltConfig, builtCardOffset[player]);
    }

    ImGui::PopID();
    ImGui::PopID();
}

void ListenerGUI::drawState()
{
    if (closed) return;

    bool advance = false;

    while (!advance)
    {
        glfwPollEvents();

        if (glfwWindowShouldClose(window))
        {
            onClose();
            return;
        }

        glClearColor(0.5, 0.5, 0.5, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("7wdai");

        pressedId = OBJ_NONE;

        drawDeck(DECK_GAME_TOKENS, tokenConfig, gameTokenRowCols.data(), gameTokenOffset, NUM_GAME_TOKENS);
        drawDeck(DECK_BOX_TOKENS, tokenConfig, boxTokenRowCols.data(), boxTokenOffset, NUM_BOX_TOKENS);
        drawDeck(DECK_REVEALED_WONDERS, wonderConfig, wonderRowCols.data(), revealedWonderOffset, NUM_WONDERS_REVEALED);

        for (int i = 0; i < NUM_PLAYERS; ++i)
        {
            drawDeck(DECK_SELECTED_WONDERS + i, wonderConfig, wonderRowCols.data(), selectedWondersOffset[i], NUM_WONDERS_PER_PLAYER);
        }

        for (int i = 0; i < NUM_PLAYERS; ++i)
        {
            drawBuilt(i);
        }

        drawPyramid();

        ImGui::SetCursorPos(ImVec2(10 * SIZE_MULT, 312 * SIZE_MULT + TOP_OFFSET));

        if (ImGui::Button("Advance"))
        {
            advance = true;
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

void glfwErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

ListenerGUI::ListenerGUI()
    : closed(false)
{
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit())
    {
        closed = true;
        exit(1);
        return;
    }

    window = glfwCreateWindow(1200, 1200, "7wdai", NULL, NULL);

    if (window == nullptr)
    {
        closed = true;
        glfwTerminate();
        exit(1);
        return;
    }

    glfwMakeContextCurrent(window);
    glViewport(0, 0, 1200, 1200);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

ListenerGUI::~ListenerGUI()
{
    if (closed) return;

    onClose();
}

void ListenerGUI::onClose()
{
    closed = true;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    exit(0);
}

void ListenerGUI::notifyStart()
{
    std::fill(isDeckCached.begin(), isDeckCached.end(), false);
    drawState();
}

void ListenerGUI::notifyAction(const Action& action)
{
    if (action.isPlayerMove()) drawState();
}

void ListenerGUI::notifyEnd()
{
    drawState();
}
