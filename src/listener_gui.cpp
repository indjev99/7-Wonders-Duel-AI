#include "listener_gui.h"

#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

struct SlotRowCol
{
    int row;
    int col;
};

const std::array<std::array<SlotRowCol, PYRAMID_SIZE>, NUM_AGES> slotRowCols = {{
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

const std::array<SlotRowCol, NUM_WONDERS_REVEALED> wonderRowCols = {{
    {0, 0},
    {0, 2},
    {1, 0},
    {1, 2}
}};

const std::array<SlotRowCol, NUM_TOKENS> gameTokenRowCols = {{
    {0, 0},
    {1, 0},
    {2, 0},
    {3, 0},
    {4, 0}
}};

#define NUM_CARD_DECKS 4

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
    ImVec4(0.98, 0.80, 0.04, 1.0),
    ImVec4(0.50, 0.37, 0.60, 1.0),
    ImVec4(0.00, 0.47, 0.27, 1.0),
    ImVec4(0.87, 0.79, 0.43, 1.0),
    ImVec4(0.00, 0.00, 0.00, 1.0)
}};

struct PosConfig
{
    ImVec2 size;
    ImVec2 sizegap;
    ImVec2 offset;
    double border;
};

const PosConfig cardConfig = {
    ImVec2(44, 68),
    ImVec2(50, 40),
    ImVec2(10, 10),
    1.5
};

const PosConfig wonderConfig = {
    ImVec2(100, 65),
    ImVec2(110, 75),
    ImVec2(52, 95),
    2.5
};

const PosConfig tokenConfig = {
    ImVec2(35, 35),
    ImVec2(40, 40),
    ImVec2(320, 65),
    1.0
};

const double TOP_OFFSET = 15;
const double SIZE_MULT = 2;

const double BORDER_DARK = 0.5;

ImVec4 borderCol(const ImVec4& col)
{
    return ImVec4(col.x * BORDER_DARK, col.y * BORDER_DARK, col.z * BORDER_DARK, col.w);
}

void drawObject(int objId, const SlotRowCol& rowCol, const PosConfig& posConfig, int deck = DECK_AGE_1)
{
    ImVec4 col = objId != OBJ_NONE ? typeCols[objects[objId].type] : deckCols[deck];
    std::string name = objId != OBJ_NONE ? objects[objId].name : "##";

    double x = posConfig.sizegap.x * rowCol.col / 2 + posConfig.offset.x;
    double y = posConfig.sizegap.y * rowCol.row + posConfig.offset.y;

    ImVec4 bCol = borderCol(col);

    ImGui::PushStyleColor(ImGuiCol_Button, bCol);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bCol);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, bCol);

    ImGui::SetCursorPos(ImVec2(x * SIZE_MULT, y * SIZE_MULT + TOP_OFFSET));
    ImGui::Button("##", ImVec2(posConfig.size.x * SIZE_MULT, posConfig.size.y * SIZE_MULT));

    ImGui::PopStyleColor(3);

    ImGui::PushStyleColor(ImGuiCol_Button, col);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, col);

    x += posConfig.border;
    y += posConfig.border;

    double sizeX = posConfig.size.x - 2 * posConfig.border;
    double sizeY = posConfig.size.y - 2 * posConfig.border;

    ImGui::SetCursorPos(ImVec2(x * SIZE_MULT, y * SIZE_MULT + TOP_OFFSET));
    ImGui::Button(name.c_str(), ImVec2(sizeX * SIZE_MULT, sizeY * SIZE_MULT));

    ImGui::PopStyleColor(3);
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

        int age = game->getCurrAge();

        for (int i = 0; i < game->getDeckSize(DECK_REVEALED_WONDERS); i++)
        {
            int objectId = game->getDeckElem(DECK_REVEALED_WONDERS, i);
            drawObject(objectId, wonderRowCols[i], wonderConfig);
        }

        for (int i = 0; i < game->getDeckSize(DECK_GAME_TOKENS); i++)
        {
            int objectId = game->getDeckElem(DECK_GAME_TOKENS, i);
            drawObject(objectId, gameTokenRowCols[i], tokenConfig);
        }

        for (int i = 0; age >= 0 && i < PYRAMID_SIZE; i++)
        {
            const PyramidSlot& slot = game->getPyramidSlot(i);
            if (slot.objectId == OBJ_NONE) continue;
            int id = slot.objectId != SLOT_UNREVEALED ? slot.objectId : OBJ_NONE;
            drawObject(id, slotRowCols[age][i], cardConfig, slot.deck);
        }

        ImGui::SetCursorPos(ImVec2(10, 10 + TOP_OFFSET));

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
