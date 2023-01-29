#include "listener_gui.h"

#include <algorithm>
#include <iostream>

#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

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

#define NUM_COL_DECKS 11
#define DECK_MLEAD_FIGURE 4
#define DECK_MLEAD_SLOT 5
#define DECK_MLEAD_FINAL_SLOT 6
#define DECK_LOOTING_TOKENS 7
#define DECK_LOOTING_SLOTS 8
#define DECK_COINS 9
#define DECK_SCORE 10

const std::array<ImVec4, NUM_COL_DECKS> deckCols = {{
    ImVec4(0.75, 0.39, 0.19, 1.0),
    ImVec4(0.00, 0.57, 0.81, 1.0),
    ImVec4(0.71, 0.58, 0.76, 1.0),
    ImVec4(0.45, 0.42, 0.68, 1.0),
    ImVec4(0.77, 0.15, 0.12, 1.0),
    ImVec4(0.79, 0.55, 0.45, 1.0),
    ImVec4(0.79, 0.72, 0.43, 1.0),
    ImVec4(0.79, 0.30, 0.20, 1.0),
    ImVec4(0.75, 0.60, 0.40, 1.0),
    ImVec4(0.92, 0.77, 0.17, 1.0),
    ImVec4(0.36, 0.73, 0.40, 1.0)
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

const ImVec4 bgColor = ImVec4(0.20, 0.20, 0.20, 1.0);
const ImVec4 textColor = ImVec4(0.00, 0.00, 0.00, 1.0);

const ListenerGUI::SpaceConfig pyramidCardConfig = {
    ImVec2(44, 68),
    ImVec2(50, 40),
    1.5,
    3.5
};

const ListenerGUI::SpaceConfig builtCardConfig = {
    ImVec2(44, 68),
    ImVec2(50, 16),
    1.5,
    3.5
};

const ListenerGUI::SpaceConfig wonderCardConfig = {
    ImVec2(68, 44),
    ImVec2(68, 44),
    1.5,
    3.5
};

const ListenerGUI::SpaceConfig discardedConfig = {
    ImVec2(44, 68),
    ImVec2(50, 74),
    1.5,
    3.5
};

const ListenerGUI::SpaceConfig wonderConfig = {
    ImVec2(100, 65),
    ImVec2(110, 75),
    2.5,
    3.5
};

const ListenerGUI::SpaceConfig tokenConfig = {
    ImVec2(35, 35),
    ImVec2(40, 40),
    1.0,
    100
};

const ListenerGUI::SpaceConfig mLeadSlotConfig = {
    ImVec2(30, 12),
    ImVec2(30, 15),
    0.75,
    100
};

const ListenerGUI::SpaceConfig lootingBaseConfig = {
    ImVec2(25, 0),
    ImVec2(30, 15),
    0.75,
    3.5
};

const ListenerGUI::SpaceConfig coinScoreConfig = {
    ImVec2(20, 20),
    ImVec2(25, 25),
    0.75,
    100
};

const ImVec2 pyramidCardOffset(10, 165);
const ImVec2 revealedWonderOffset(52, 249);
const ImVec2 wonderCardRelOffset(37, 10.5);
const ImVec2 gameTokenOffset(395, 221.5);
const ImVec2 boxTokenOffset(440, 261.5);
const ImVec2 discardedOffset(10, 643);
const ImVec2 mLeadOffset(350, 313);

const std::array<ImVec2, NUM_PLAYERS> selectedWondersOffset = {{
    ImVec2(10, 488),
    ImVec2(10, 10)
}};

const std::array<ImVec2, NUM_PLAYERS> builtCardOffset = {{
    ImVec2(230, 488),
    ImVec2(230, 82)
}};

const std::array<ImVec2, NUM_PLAYERS> builtTokenHalfRelOffset = {{
    ImVec2(10, 488),
    ImVec2(10, 115)
}};

const std::array<ImVec2, NUM_PLAYERS> coinScoreOffset = {{
    ImVec2(10, 463),
    ImVec2(10, 155)
}};


const ImVec2 tokenTextPos = ImVec2(0.5, 0.5);
const ImVec2 cardWonderTextPos = ImVec2(0.5, 0.0);

const int MAX_BUILT_CARD_PER_COL = 5;
const int MAX_BUILT_TOKEN_PER_COL = 3;
const int MAX_DISCARDED_PER_ROW = 12;

const double BORDER_DARK = 0.5;

float SIZE_MULT = 2;

ImVec4 borderCol(const ImVec4& col)
{
    return ImVec4(col.x * BORDER_DARK, col.y * BORDER_DARK, col.z * BORDER_DARK, col.w);
}

#define NO_TEXTURE (GLuint) -1

GLuint LoadTextureFromFile(const std::string& objName)
{
    std::string fileName = objName;
    std::transform(fileName.begin(), fileName.end(), fileName.begin(), [](char c){ return (c == ' ') ? '_' : std::tolower(c); });

    fileName = "imgs/" + fileName + ".png";

    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(fileName.c_str(), &image_width, &image_height, nullptr, 4);

    if (image_data == nullptr) return NO_TEXTURE;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    return image_texture;
}

bool myImageButton(const char* str_id, ImTextureID texture_id, const ImVec2& size, ImGuiButtonFlags flags = 0, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1))
{
    using namespace ImGui;

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    ImGuiID id = window->GetID(str_id);

    const ImVec2 padding = g.Style.FramePadding;
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size + padding * 2.0f);
    ItemSize(bb);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, g.Style.FrameRounding));
    if (bg_col.w > 0.0f)
        window->DrawList->AddRectFilled(bb.Min + padding, bb.Max - padding, GetColorU32(bg_col));
    window->DrawList->AddImage(texture_id, bb.Min + padding, bb.Max - padding, uv0, uv1, GetColorU32(tint_col));

    return pressed;
}

void ListenerGUI::drawObject(int objId, const ListenerGUI::SlotRowCol& rowCol, const ListenerGUI::SpaceConfig& spaceConfig, const ImVec2& offset, int deck, const std::string& extra)
{
    if (objId == OBJ_NONE && deck == DECK_NONE) return;

    bool pressed = false;

    ImVec4 col = objId != OBJ_NONE ? typeCols[objects[objId].type] : deckCols[deck];
    std::string name = extra + (objId != OBJ_NONE ? objects[objId].name : "##");

    ImVec2 pos = spaceConfig.sizegap * ImVec2(rowCol.col / 2.0, rowCol.row) + offset;

    ImGui::PushID(rowCol.row);
    ImGui::PushID(rowCol.col);

    if (objId != OBJ_NONE && objects[objId].type == OT_WONDER && deck != DECK_NONE)
    {
        ImGui::PushID("WONDER CARD");
        drawObject(OBJ_NONE, {0, 0}, wonderCardConfig, pos + wonderCardRelOffset, deck);
        ImGui::PopID();
    }

    if (objId == OBJ_NONE || objectTextures[objId] == NO_TEXTURE)
    {
        ImVec4 bCol = borderCol(col);
        ImVec2 textPos = objId == OBJ_NONE || objects[objId].type == OT_TOKEN ? tokenTextPos : cardWonderTextPos;

        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, textPos);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, spaceConfig.rounding);

        ImGui::PushStyleColor(ImGuiCol_Button, bCol);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bCol);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, bCol);

        ImGui::SetCursorPos(pos * SIZE_MULT);
        pressed = pressed || ImGui::ButtonEx(("## Border of " + name).c_str(), spaceConfig.size * SIZE_MULT, ImGuiButtonFlags_AllowItemOverlap);
        ImGui::SetItemAllowOverlap();

        ImGui::PopStyleColor(3);

        ImVec2 borderVec(spaceConfig.border, spaceConfig.border);

        pos += borderVec;

        ImVec2 size = spaceConfig.size - borderVec * 2;

        ImGui::PushStyleColor(ImGuiCol_Button, col);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, col);
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);

        ImGui::SetCursorPos(pos * SIZE_MULT);
        pressed = pressed || ImGui::ButtonEx(name.c_str(), size * SIZE_MULT, ImGuiButtonFlags_AllowItemOverlap);
        ImGui::SetItemAllowOverlap();

        ImGui::PopStyleColor(4);

        ImGui::PopStyleVar(2);
    }
    else
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 100);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        
        ImGui::SetCursorPos(pos * SIZE_MULT);
        pressed = pressed || myImageButton(name.c_str(), (void*) (intptr_t) objectTextures[objId], spaceConfig.size * SIZE_MULT, ImGuiButtonFlags_AllowItemOverlap);
        ImGui::SetItemAllowOverlap();

        ImGui::PopStyleVar(2);
    }

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
        drawObject(id, slotRowCols[age][i], pyramidCardConfig, pyramidCardOffset, slot.deck);
    }

    ImGui::PopID();
}

void ListenerGUI::drawBuilt(int player)
{
    const PlayerState& state = game->getPlayerState(player);

    std::vector<int> cntByCol;
    std::array<int, NUM_OBJECTS> objCol;
    std::array<int, NUM_OBJECTS> objRow;

    int currType = OT_NONE;
    int currCol = -1;
    int lastCardCol = -1;

    for (int i = 0; i < NUM_OBJECTS; ++i)
    {
        int type = objects[i].type;

        if (type == OT_WONDER) continue;
        if (type == OT_LOOTING) continue;
        if (!state.objectsBuilt[i]) continue;

        int maxInCol = type == OT_TOKEN ? MAX_BUILT_TOKEN_PER_COL : MAX_BUILT_CARD_PER_COL;

        if (type != currType || cntByCol[currCol] == maxInCol)
        {
            currType = type;
            currCol++;
            cntByCol.push_back(0);
        }

        if (type != OT_TOKEN) lastCardCol = currCol;

        objCol[i] = currCol;
        objRow[i] = cntByCol[currCol];

        cntByCol[currCol]++;
    }

    ImGui::PushID("BUILT");
    ImGui::PushID(player);

    for (int i = 0; i < NUM_OBJECTS; ++i)
    {
        int type = objects[i].type;

        if (type == OT_LOOTING) continue;
        if (!state.objectsBuilt[i]) continue;

        if (type == OT_WONDER)
        {
            drawObject(i, cachedRowCols[i], wonderConfig, selectedWondersOffset[player], wonderBuiltWithDeck[i]);
            continue;
        }

        int col = objCol[i];
        int row = objRow[i];

        if (player != 0) row -= cntByCol[col] - 1;

        if (type == OT_TOKEN)
        {
            double oX = builtCardOffset[player].x +
                (lastCardCol == -1 ? 0 : lastCardCol * builtCardConfig.sizegap.x + builtCardConfig.size.x + builtTokenHalfRelOffset[player].x);
            double oY = builtTokenHalfRelOffset[player].y;
            drawObject(i, SlotRowCol{row, 2 * (col - lastCardCol - 1)}, tokenConfig, ImVec2(oX, oY));
            continue;
        }

        drawObject(i, SlotRowCol{row, 2 * col}, builtCardConfig, builtCardOffset[player]);
    }

    ImGui::PushID("COIN SCORE");

    drawObject(OBJ_NONE, SlotRowCol{0, 0}, coinScoreConfig, coinScoreOffset[player], DECK_COINS, std::to_string(state.coins));
    drawObject(OBJ_NONE, SlotRowCol{0, 2}, coinScoreConfig, coinScoreOffset[player], DECK_SCORE, std::to_string(state.getScore()));

    ImGui::PopID();

    ImGui::PopID();
    ImGui::PopID();
}

void ListenerGUI::drawDiscarded()
{
    ImGui::PushID(DECK_DISCARDED);

    for (int i = 0; i < game->getDeckSize(DECK_DISCARDED); i++)
    {
        int objectId = game->getDeckElem(DECK_DISCARDED, i);
        SlotRowCol rowCol = {i / MAX_DISCARDED_PER_ROW, i % MAX_DISCARDED_PER_ROW * 2};
        drawObject(objectId, rowCol, discardedConfig, discardedOffset);
    }

    ImGui::PopID();
}

void ListenerGUI::drawLooting(int from, int to, int deck)
{
    if (from < to) std::swap(from, to);

    SpaceConfig config = lootingBaseConfig;
    config.size.y = (from - to) * mLeadSlotConfig.sizegap.y + mLeadSlotConfig.size.y;

    ImGui::PushID(deck);
    ImGui::PushID(from);

    drawObject(OBJ_NONE, {to, -2}, config, mLeadOffset, deck);

    ImGui::PopID();
    ImGui::PopID();
}

void ListenerGUI::drawMilitaryLead()
{
    ImGui::PushID("MLEAD SLOTS");

    for (int i = - MILITARY_THRESHOLD_WIN; i <= MILITARY_THRESHOLD_WIN; i++)
    {
        drawObject(OBJ_NONE, {-i, 0}, mLeadSlotConfig, mLeadOffset, std::abs(i) < MILITARY_THRESHOLD_WIN ? DECK_MLEAD_SLOT : DECK_MLEAD_FINAL_SLOT);
    }

    ImGui::PopID();

    ImGui::PushID("MLEAD FIGURE");

    int mLead = game->getMilitaryLead(0);
    mLead = std::min(MILITARY_THRESHOLD_WIN, mLead);
    mLead = std::max(- MILITARY_THRESHOLD_WIN, mLead);

    drawObject(OBJ_NONE, {-mLead, 0}, mLeadSlotConfig, mLeadOffset, DECK_MLEAD_FIGURE);

    ImGui::PopID();

    ImGui::PushID("LOOTING");

    for (int i = 0; i < NUM_PLAYERS; ++i)
    {
        const PlayerState& state = game->getPlayerState(i);
        int sign = i == 0 ? -1 : 1;
        drawLooting(sign * MILITARY_THRESHOLD_1, sign * (MILITARY_THRESHOLD_2 - 1), DECK_LOOTING_SLOTS);
        drawLooting(sign * MILITARY_THRESHOLD_2, sign * (MILITARY_THRESHOLD_3 - 1), DECK_LOOTING_SLOTS);
        drawLooting(sign * MILITARY_THRESHOLD_3, sign * (MILITARY_THRESHOLD_WIN - 1), DECK_LOOTING_SLOTS);
        if (!state.objectsBuilt[O_LOOTING_LOOTING_1]) drawLooting(sign * MILITARY_THRESHOLD_2, sign * (MILITARY_THRESHOLD_3 - 1), DECK_LOOTING_TOKENS);
        if (!state.objectsBuilt[O_LOOTING_LOOTING_2]) drawLooting(sign * MILITARY_THRESHOLD_3, sign * (MILITARY_THRESHOLD_WIN - 1), DECK_LOOTING_TOKENS);
    }

    ImGui::PopID();
}

void ListenerGUI::drawState(bool canAdvance)
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
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColor);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::Begin("7wdai", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

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
        drawDiscarded();
        drawMilitaryLead();

        if (canAdvance)
        {
            ImGui::SetCursorPos(ImVec2(480 * SIZE_MULT, 315 * SIZE_MULT));
            advance = ImGui::ArrowButton("##Advance", ImGuiDir_::ImGuiDir_Right);
        }

        ImGui::End();
        ImGui::PopStyleVar(1);
        ImGui::PopStyleColor(1);
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
        return;
    }

    window = glfwCreateWindow(1200, 1300, "7wdai", NULL, NULL);

    if (window == nullptr)
    {
        closed = true;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glViewport(0, 0, 1200, 1300);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    for (int i = 0; i < NUM_OBJECTS; ++i)
    {
        objectTextures[i] = LoadTextureFromFile(objects[i].name);
    }
}

int findDeck(int id)
{
    if (std::find(age1Cards.begin(), age1Cards.end(), id) != age1Cards.end()) return DECK_AGE_1;
    if (std::find(age2Cards.begin(), age2Cards.end(), id) != age2Cards.end()) return DECK_AGE_2;
    if (std::find(age3Cards.begin(), age3Cards.end(), id) != age3Cards.end()) return DECK_AGE_3;
    if (std::find(guildCards.begin(), guildCards.end(), id) != guildCards.end()) return DECK_GUILDS;
    return DECK_NONE;
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
}

void ListenerGUI::notifyStart()
{
    std::fill(isDeckCached.begin(), isDeckCached.end(), false);
}

void ListenerGUI::notifyAction(const Action& action)
{
    if (action.isPlayerMove()) drawState();
    if (action.type == ACT_MOVE_PLAY_PYRAMID_CARD && action.arg2 >= 0)
    {
        wonderBuiltWithDeck[action.arg2] = findDeck(action.arg1);
    }
}

void ListenerGUI::notifyEnd()
{
    drawState(false);
}
