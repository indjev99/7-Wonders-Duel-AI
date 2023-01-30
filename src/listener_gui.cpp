#include "listener_gui.h"

#include "player_gui.h"

#include <algorithm>
#include <iostream>

#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

const std::array<int, NUM_OBJECTS> objectDrawOrder = {{
    O_BROWN_LUMBER_YARD,
    O_BROWN_LOGGING_CAMP,
    O_BROWN_SAWMILL,
    O_BROWN_CLAY_POOL,
    O_BROWN_CLAY_PIT,
    O_BROWN_BRICKYARD,
    O_BROWN_QUARRY,
    O_BROWN_STONE_PIT,
    O_BROWN_SHELF_QUARRY,
    O_GRAY_GLASSWORKS,
    O_GRAY_GLASSBLOWER,
    O_GRAY_PRESS,
    O_GRAY_DRYING_ROOM,
    O_YELLOW_TAVERN,
    O_YELLOW_BREWERY,
    O_YELLOW_STONE_RESERVE,
    O_YELLOW_CLAY_RESERVE,
    O_YELLOW_WOOD_RESERVE,
    O_YELLOW_CUSOMS_HOUSE,
    O_YELLOW_CARAVANSERY,
    O_YELLOW_FORUM,
    O_YELLOW_LIGHTHOUSE,
    O_YELLOW_ARENA,
    O_YELLOW_CHAMBER_OF_COMMERCE,
    O_YELLOW_PORT,
    O_YELLOW_ARMORY,
    O_RED_STABLE,
    O_RED_GARRISON,
    O_RED_PALISADE,
    O_RED_GUARD_TOWER,
    O_RED_HORSE_BREEDERS,
    O_RED_BARRACKS,
    O_RED_ARCHERY_RANGE,
    O_RED_PARADE_GROUND,
    O_RED_WALLS,
    O_RED_FORTIFICATIONS,
    O_RED_SIEGE_WORKSHOP,
    O_RED_CIRCUS,
    O_RED_ARSENAL,
    O_RED_COURTHOUSE,
    O_GREEN_SCRIPTORIUM,
    O_GREEN_LIBRARY,
    O_GREEN_PHARMACIST,
    O_GREEN_DISPENSARY,
    O_GREEN_WORKSHOP,
    O_GREEN_LABORATORY,
    O_GREEN_APOTHECARY,
    O_GREEN_SCHOOL,
    O_GREEN_UNIVERSITY,
    O_GREEN_OBSERVATORY,
    O_GREEN_ACADEMY,
    O_GREEN_STUDY,
    O_BLUE_THEATER,
    O_BLUE_ALTAR,
    O_BLUE_BATHS,
    O_BLUE_STATUE,
    O_BLUE_TEMPLE,
    O_BLUE_ROSTRUM,
    O_BLUE_AQUEDUCT,
    O_BLUE_TRIBUNAL,
    O_BLUE_SENATE,
    O_BLUE_OBELISK,
    O_BLUE_GARDENS,
    O_BLUE_PANTHEON,
    O_BLUE_PALACE,
    O_BLUE_TOWN_HALL,
    O_GUILD_MERCHANTS_GUILD,
    O_GUILD_SHIPOWNERS_GUILD,
    O_GUILD_BUILDERS_GUILD,
    O_GUILD_MAGISTRATES_GUILD,
    O_GUILD_SCIENTISTS_GUILD,
    O_GUILD_TACTICIANS_GUILD,
    O_GUILD_MONEYLENDERS_GUILD,
    O_TOKEN_AGRICULTURE,
    O_TOKEN_ARCHITECTURE,
    O_TOKEN_ECONOMY,
    O_TOKEN_LAW,
    O_TOKEN_MASONRY,
    O_TOKEN_MATHEMATICS,
    O_TOKEN_PHILOSOPHY,
    O_TOKEN_STRATEGY,
    O_TOKEN_THEOLOGY,
    O_TOKEN_URBANISM,
    O_WONDER_THE_APPIAN_WAY,
    O_WONDER_CIRCUS_MAXIMUS,
    O_WONDER_THE_COLOSSUS,
    O_WONDER_THE_GREAT_LIBRARY,
    O_WONDER_THE_GREAT_LIGHTHOUSE,
    O_WONDER_THE_HANGING_GARDENS,
    O_WONDER_THE_MAUSOLEUM,
    O_WONDER_PIRAEUS,
    O_WONDER_THE_PYRAMIDS,
    O_WONDER_THE_SPHINX,
    O_WONDER_THE_STATUE_OF_ZEUS,
    O_WONDER_THE_TEMPLE_OF_ARTEMIS,
    O_LOOTING_LOOTING_1,
    O_LOOTING_LOOTING_2
}};

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

const ImVec4 bgColor = ImVec4(0.80, 0.75, 0.60, 1.0);
const ImVec4 transColor = ImVec4(0.00, 0.00, 0.00, 0.0);
const ImVec4 textColor = ImVec4(0.00, 0.00, 0.00, 1.0);
const ImVec4 badTextColor = ImVec4(0.90, 0.10, 0.10, 1.0);
const ImVec4 goodTextColor = ImVec4(0.10, 0.90, 0.10, 1.0);
const ImVec4 nonHighlightTintColor = ImVec4(1.00, 1.00, 1.00, 1.0);
const ImVec4 highlightTintColor = ImVec4(0.75, 0.75, 0.75, 1.0);

const ListenerGUI::SpaceConfig pyramidCardConfig = {
    ImVec2(44, 68),
    ImVec2(50, 40)
};

const ListenerGUI::SpaceConfig builtCardConfig = {
    ImVec2(44, 68),
    ImVec2(50, 17)
};

const ListenerGUI::SpaceConfig wonderCardConfig = {
    ImVec2(68, 44),
    ImVec2(68, 44)
};

const ListenerGUI::SpaceConfig discardedConfig = {
    ImVec2(44, 68),
    ImVec2(50, 74)
};

const ListenerGUI::SpaceConfig wonderConfig = {
    ImVec2(100, 65),
    ImVec2(115, 71)
};

const ListenerGUI::SpaceConfig tokenConfig = {
    ImVec2(35, 35),
    ImVec2(37.7, 37.7)
};

const ListenerGUI::SpaceConfig mLeadConfig = {
    ImVec2(35.3, 17),
    ImVec2(35.3, 17.9)
};

const ListenerGUI::SpaceConfig looting1Config = {
    ImVec2(17, 38),
    ImVec2(17, 38)
};

const ListenerGUI::SpaceConfig looting2Config = {
    ImVec2(20, 45),
    ImVec2(20, 45)
};

const ListenerGUI::SpaceConfig statsConfig = {
    ImVec2(20, 20),
    ImVec2(25, 25)
};

const ListenerGUI::SpaceConfig smallstatsConfig = {
    ImVec2(12, 12),
    ImVec2(12, 12)
};

const ListenerGUI::SpaceConfig sideBoardConfig = {
    ImVec2(104.5, 370),
    ImVec2(104.5, 370)
};

const ImVec2 pyramidCardOffset(200, 156);
const ImVec2 revealedWonderOffset(242, 240);
const ImVec2 wonderCardRelOffset(42, 10.5);
const ImVec2 gameTokenOffset(817.3, 214.1);
const ImVec2 boxTokenOffset(705, 251.8);
const ImVec2 discardedOffset(10, 620);
const ImVec2 mLeadOffset(776.9, 298.5);
const ImVec2 sideBoardOffset(750, 122.5);
const ImVec2 wonderCostRelOffset(96, 25.5);

const std::array<ImVec2, NUM_PLAYERS> selectedWondersOffset = {{
    ImVec2(10, 474),
    ImVec2(10, 10)
}};

const std::array<ImVec2, NUM_PLAYERS> builtCardOffset = {{
    ImVec2(240, 474),
    ImVec2(240, 78)
}};

const std::array<ImVec2, NUM_PLAYERS> builtTokenHalfRelOffset = {{
    ImVec2(10, 474),
    ImVec2(10, 111)
}};

const std::array<ImVec2, NUM_PLAYERS> looting1Offset = {{
    ImVec2(759.8, 218),
    ImVec2(759.8, 359.7)
}};

const std::array<ImVec2, NUM_PLAYERS> looting2Offset = {{
    ImVec2(758.4, 160.5),
    ImVec2(758.4, 410.1)
}};

const std::array<ImVec2, NUM_PLAYERS> statsOffset = {{
    ImVec2(10, 449),
    ImVec2(10, 151)
}};

const std::array<ImVec2, NUM_PLAYERS> cardCostRelOffset = {{
    ImVec2(2, 54),
    ImVec2(30, 54)
}};

const int MAX_BUILT_CARD_PER_COL = 5;
const int MAX_BUILT_TOKEN_PER_COL = 3;
const int MAX_DISCARDED_PER_ROW = 17;

const double SIZE_MULT = 1.5;
const int MAIN_FONT_SIZE = 22;
const int SMALL_FONT_SIZE = 17;

const int INIT_WINDOW_W = 1800;
const int INIT_WINDOW_H = 1350;

bool myImageButton(const char* str_id, ImTextureID texture_id, const ImVec2& size, const char* label = "", ImGuiButtonFlags flags = 0, const ImVec4& tint_col = ImVec4(1, 1, 1, 1))
{
    using namespace ImGui;

    ImVec2 uv0 = ImVec2(0, 0);
    ImVec2 uv1 = ImVec2(1, 1);
    ImVec4 bg_col = ImVec4(0, 0, 0, 0);

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    ImGuiID id = window->GetID(str_id);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

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

    RenderTextClipped(bb.Min + padding, bb.Max - padding, label, NULL, &label_size, g.Style.ButtonTextAlign, &bb);

    return pressed;
}

GLuint ListenerGUI::loadTexture(const std::string& objName)
{
    std::string fileName = objName;
    std::transform(fileName.begin(), fileName.end(), fileName.begin(), [](char c){ return (c == ' ') ? '_' : std::tolower(c); });

    fileName = "resources/" + fileName + ".png";

    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(fileName.c_str(), &image_width, &image_height, nullptr, 4);

    if (image_data == nullptr) return TEXTURE_NONE;

    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    return image_texture;
}

bool ListenerGUI::drawCost(int objId, int player, const ImVec2& offset)
{
    ImGui::PushID("COST");
    ImGui::PushID(player);
    ImGui::PushFont(fonts[SMALL_FONT]);

    const PlayerState& state = game->getPlayerState(player);
    int cost = state.getCost(objects[objId]);

    ImVec4 col = cost <= 0 ? goodTextColor : cost > state.coins ? badTextColor : textColor;

    ImGui::PushStyleColor(ImGuiCol_Text, col);
    bool pressed = drawObject(O_TEXTURE_COINS, {0, 0}, smallstatsConfig, offset, std::to_string(cost));
    ImGui::PopStyleColor();

    ImGui::PopFont();
    ImGui::PopID();
    ImGui::PopID();

    return pressed;
}

bool ListenerGUI::drawObject(int objId, const ListenerGUI::SlotRowCol& rowCol, const ListenerGUI::SpaceConfig& spaceConfig, const ImVec2& offset, const std::string& text)
{
    bool pressed = false;

    std::string name = objId < NUM_OBJECTS ? objects[objId].name : "##";
    GLint texture = objectTextures[objId];

    ImVec2 pos = spaceConfig.sizegap * ImVec2(rowCol.col / 2.0, rowCol.row) + offset;

    ImGui::PushID(objId);
    ImGui::PushID(rowCol.row);
    ImGui::PushID(rowCol.col);

    if (objId < NUM_OBJECTS && objects[objId].type == OT_WONDER && game->getObjectDeck(objId) == DECK_USED)
    {
        ImGui::PushID("WONDER CARD");
        drawObject(O_TEXTURE_DECKS_ROTATED + wonderBuiltWithDeck[objId], {0, 0}, wonderCardConfig, pos + wonderCardRelOffset);
        ImGui::PopID();
    }

    if (texture == TEXTURE_NONE)
    {
        ImGui::SetCursorPos(pos * SIZE_MULT);
        pressed = ImGui::ButtonEx(name.c_str(), spaceConfig.size * SIZE_MULT, ImGuiButtonFlags_AllowItemOverlap);
        ImGui::SetItemAllowOverlap();
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, transColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, transColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, transColor);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        ImGui::SetCursorPos(pos * SIZE_MULT);
        ImVec4 tintCol = isHighlighted[objId] ? highlightTintColor : nonHighlightTintColor;
        pressed = myImageButton(name.c_str(), (void*) (intptr_t) texture, spaceConfig.size * SIZE_MULT, text.data(), ImGuiButtonFlags_AllowItemOverlap, tintCol);
        ImGui::SetItemAllowOverlap();

        ImGui::PopStyleVar(1);

        ImGui::PopStyleColor(3);
    }

    if (objId < NUM_OBJECTS && game->isPlayableCard(objId))
    {
        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            pressed = pressed || drawCost(objId, i, pos + cardCostRelOffset[i]);
        }
    }

    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        if (objId < NUM_OBJECTS && game->getObjectDeck(objId) == DECK_SELECTED_WONDERS + i)
        {
            pressed = pressed || drawCost(objId, i, pos + wonderCostRelOffset);
        }
    }

    ImGui::PopID();
    ImGui::PopID();
    ImGui::PopID();

    if (pressed)
    {
        std::cerr << "Pressed: " << objId << " " << objects[objId].name << std::endl;
        pressedId = objId;
    }

    return pressed;
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
        int id = slot.objectId != SLOT_UNREVEALED ? slot.objectId : O_TEXTURE_DECKS + slot.deck;
        drawObject(id, slotRowCols[age][i], pyramidCardConfig, pyramidCardOffset);
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

    for (int i : objectDrawOrder)
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

    for (int i : objectDrawOrder)
    {
        int type = objects[i].type;

        if (type == OT_LOOTING) continue;
        if (!state.objectsBuilt[i]) continue;

        if (type == OT_WONDER)
        {
            drawObject(i, cachedRowCols[i], wonderConfig, selectedWondersOffset[player]);
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

    ImGui::PopID();
    ImGui::PopID();
}


void ListenerGUI::drawPlayerStats(int player)
{
    const PlayerState& state = game->getPlayerState(player);

    ImGui::PushID("STATS");
    ImGui::PushID(player);

    drawObject(O_TEXTURE_COINS, SlotRowCol{0, 0}, statsConfig, statsOffset[player], std::to_string(state.coins));
    drawObject(O_TEXTURE_SCORE, SlotRowCol{0, 2}, statsConfig, statsOffset[player], std::to_string(state.getScore()));

    bool hasArrow = !game->isTerminal() ? game->getCurrActor() == player : game->getResult(player) > 0;

    if (hasArrow)
        drawObject(O_TEXTURE_PLAYER_ARROWS + player, SlotRowCol{0, 4}, statsConfig, statsOffset[player]);

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

void ListenerGUI::drawMilitaryLead()
{
    drawObject(O_TEXTURE_SIDE_BOARD, {0, 0}, sideBoardConfig, sideBoardOffset);

    ImGui::PushID("MILITARY LEAD");

    int mLead = game->getMilitaryLead(0);
    mLead = std::min(MILITARY_THRESHOLD_WIN, mLead);
    mLead = std::max(- MILITARY_THRESHOLD_WIN, mLead);

    drawObject(O_TEXTURE_MILITARY_LEAD, {-mLead, 0}, mLeadConfig, mLeadOffset);

    ImGui::PopID();

    ImGui::PushID("LOOTING");

    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        const PlayerState& state = game->getPlayerState(i);
        ImGui::PushID(i);
        if (!state.objectsBuilt[O_LOOTING_LOOTING_1]) drawObject(O_LOOTING_LOOTING_1, {0, 0}, looting1Config, looting1Offset[i]);
        if (!state.objectsBuilt[O_LOOTING_LOOTING_2]) drawObject(O_LOOTING_LOOTING_2, {0, 0}, looting2Config, looting2Offset[i]);
        ImGui::PopID();
    }

    ImGui::PopID();
}

void ListenerGUI::drawState(bool advanceButton, bool fastAdvance, PlayerGUI* playerGui)
{
    if (closed) return;

    bool advance = false;
    bool advanceNext = fastAdvance;

    ImGuiStyle* style = &ImGui::GetStyle();
    style->Colors[ImGuiCol_Text] = textColor;

    while (!advance)
    {
        advance = advanceNext;

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
        ImGui::PushFont(fonts[MAIN_FONT]);
        ImGui::Begin("7wdai", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

        pressedId = OBJ_NONE;

        drawPyramid();
        drawDiscarded();
        drawMilitaryLead();

        drawDeck(DECK_GAME_TOKENS, tokenConfig, gameTokenRowCols.data(), gameTokenOffset, NUM_GAME_TOKENS);
        drawDeck(DECK_BOX_TOKENS, tokenConfig, boxTokenRowCols.data(), boxTokenOffset, NUM_BOX_TOKENS);
        drawDeck(DECK_REVEALED_WONDERS, wonderConfig, wonderRowCols.data(), revealedWonderOffset, NUM_WONDERS_REVEALED);

        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            drawBuilt(i);
            drawPlayerStats(i);
            drawDeck(DECK_SELECTED_WONDERS + i, wonderConfig, wonderRowCols.data(), selectedWondersOffset[i], NUM_WONDERS_PER_PLAYER);
        }

        if (advanceButton && !advance)
        {
            ImGui::SetCursorPos(ImVec2(10 * SIZE_MULT, 304 * SIZE_MULT));
            advance = ImGui::ArrowButton("##Advance", ImGuiDir_::ImGuiDir_Right);
        }

        if (playerGui != nullptr && !advance)
        {
            advance = playerGui->guiCanAdvance();
        }

        ImGui::End();
        ImGui::PopFont();
        ImGui::PopStyleVar(1);
        ImGui::PopStyleColor(1);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    if (playerGui) lastMoveWasFromGui = true;
}

void glfwErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

void ListenerGUI::loadDeckTexture(int id, const std::string& objName)
{
    objectTextures[O_TEXTURE_DECKS + id] = loadTexture(objName);
    objectTextures[O_TEXTURE_DECKS_ROTATED + id] = loadTexture(objName + " Rotated");
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

    window = glfwCreateWindow(INIT_WINDOW_W, INIT_WINDOW_H, "7wdai", NULL, NULL);

    if (window == nullptr)
    {
        closed = true;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glViewport(0, 0, INIT_WINDOW_W, INIT_WINDOW_H);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    fonts[MAIN_FONT] = io.Fonts->AddFontFromFileTTF("resources/Roboto-Medium.ttf", MAIN_FONT_SIZE);
    fonts[SMALL_FONT] = io.Fonts->AddFontFromFileTTF("resources/Roboto-Medium.ttf", SMALL_FONT_SIZE);

    std::fill(objectTextures.begin(), objectTextures.end(), TEXTURE_NONE);

    for (int i = 0; i < NUM_OBJECTS; i++)
    {
        objectTextures[i] = loadTexture(objects[i].name);
    }

    loadDeckTexture(DECK_AGE_1, "Deck Age 1");
    loadDeckTexture(DECK_AGE_2, "Deck Age 2");
    loadDeckTexture(DECK_AGE_3, "Deck Age 3");
    loadDeckTexture(DECK_GUILDS, "Deck Guilds");
    loadDeckTexture(DECK_TOKENS, "Deck Tokens");
    loadDeckTexture(DECK_WONDERS, "Deck Wonders");

    objectTextures[O_TEXTURE_PLAYER_ARROWS + 0] = loadTexture("Arrow Down");
    objectTextures[O_TEXTURE_PLAYER_ARROWS + 1] = loadTexture("Arrow Up");

    objectTextures[O_TEXTURE_SIDE_BOARD] = loadTexture("Side Board");
    objectTextures[O_TEXTURE_MILITARY_LEAD] = loadTexture("Military Lead");
    objectTextures[O_TEXTURE_COINS] = loadTexture("Coins");
    objectTextures[O_TEXTURE_SCORE] = loadTexture("Score");
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
    lastMoveWasFromGui = false;
    std::fill(isDeckCached.begin(), isDeckCached.end(), false);
}

void ListenerGUI::notifyActionPre(const Action& action)
{
    if (action.isPlayerMove() && !lastMoveWasFromGui)
    {
        std::fill(isHighlighted.begin(), isHighlighted.end(), false);

        if (action.type != ACT_MOVE_CHOOSE_START_PLAYER) isHighlighted[action.arg1] = true;
        if (action.type == ACT_MOVE_PLAY_PYRAMID_CARD && action.arg2 >= 0) isHighlighted[action.arg2] = true;

        drawState();
    }

    if (action.type == ACT_MOVE_PLAY_PYRAMID_CARD && action.arg2 >= 0)
    {
        wonderBuiltWithDeck[action.arg2] = findDeck(action.arg1);
    }

    lastMoveWasFromGui = false;
}

void ListenerGUI::notifyActionPost(const Action& action)
{
    if (game->isTerminal() || !game->getExpectedAction().isPlayerMove()) return;

    std::fill(isHighlighted.begin(), isHighlighted.end(), false);

    drawState(false, true);
}

void ListenerGUI::notifyEnd()
{
    std::fill(isHighlighted.begin(), isHighlighted.end(), false);
    drawState(false);
}
