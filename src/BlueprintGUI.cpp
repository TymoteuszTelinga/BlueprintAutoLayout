
#include "BlueprintGUI.h"

#include <iostream>
#include <rlgl.h>
#include <raymath.h>

#include <nfd.h>

BlueprintGUI::BlueprintGUI()
{
    NFD_Init();

    InitWindow(c_WindowWidth, c_WindowHeight, "BluePrint Layout");

    m_CameraPos = Vector2(c_WindowWidth/2, c_WindowHeight/2);

    m_Camera.target = m_CameraPos;
    m_Camera.offset = Vector2(c_WindowWidth/2,c_WindowHeight/2);
    m_Camera.rotation = 0.f;
    m_Camera.zoom = 1.f;

    m_AutoLayout.GenerateData();
    m_AutoLayout.CreateLayout();
}

BlueprintGUI::~BlueprintGUI()
{
    CloseWindow();

    NFD_Quit();
}

void BlueprintGUI::Run()
{
    while (!WindowShouldClose())
    {
        m_MousePosition = GetMousePosition();
        bMousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        float dt = GetFrameTime();

        OnUpdate(dt);

        OnRender();
    }
}

void BlueprintGUI::OnUpdate(float deltaTime)
{
    // --- Move Camera ---
    if (IsKeyDown(KEY_D)) 
        m_CameraPos.x += c_CameraSpeed * deltaTime;
    if (IsKeyDown(KEY_A)) 
        m_CameraPos.x -= c_CameraSpeed * deltaTime;
    if (IsKeyDown(KEY_S)) 
        m_CameraPos.y += c_CameraSpeed * deltaTime;
    if (IsKeyDown(KEY_W)) 
        m_CameraPos.y -= c_CameraSpeed * deltaTime;
    // --- Zoom ---
    m_Camera.zoom += GetMouseWheelMove() * 0.1f;
    m_Camera.zoom = Clamp(m_Camera.zoom, 0.1f, 5.0f);

    m_Camera.target = m_CameraPos;
}

void BlueprintGUI::OnRender()
{
    BeginDrawing();
    ClearBackground(DARKGRAY);

    BeginMode2D(m_Camera);
    const auto& nodesView = m_AutoLayout.GetNodes();
    const auto& edgesView = m_AutoLayout.GetEdges();
    for (const Edge& edge : edgesView)
    {
        const Node& from = nodesView.at(edge.From);
        const Node& to = nodesView.at(edge.To);
        DrawLineBezier({ from.PosX * c_StepSizeX, from.PosY * c_StepSizeY }, { to.PosX * c_StepSizeX, to.PosY * c_StepSizeY }, 5.f, WHITE);
    }
    for (const auto& [id, node] : nodesView)
    {
        DrawNode(node, 20);
    }
    EndMode2D();

    std::string cameraText = std::format("Zoom: {:.1f}", m_Camera.zoom);
    DrawText(cameraText.c_str(), 20, c_WindowHeight - 20 - 20, 20, LIGHTGRAY);

    if (Button(Rectangle(10, 10, 100, 40), "Open", 20))
    {
        OpenFile();
    }
    if (Button(Rectangle(10, 60, 100, 40), "Save", 20))
    {
        SaveFile();
    }

    EndDrawing();
}

bool BlueprintGUI::Button(const Rectangle& rect, const std::string& text, uint32_t fontSize)
{
    Matrix trasform = rlGetMatrixTransform();
    Matrix inv = MatrixInvert(trasform);

    Vector2 localMouse = {
        inv.m0 * m_MousePosition.x + inv.m4 * m_MousePosition.y + inv.m12,
        inv.m1 * m_MousePosition.x + inv.m5 * m_MousePosition.y + inv.m13
    };

    bool bHover = CheckCollisionPointRec(localMouse, rect);

    int textWidth = MeasureText(text.c_str(), fontSize);
    int textHeight = fontSize;

    int textX = (int)(rect.x + (rect.width - textWidth) / 2);
    int textY = (int)(rect.y + (rect.height - textHeight) / 2);

    DrawRectangleRounded(rect, 0.2f, 10, bHover ? GRAY : LIGHTGRAY);
    DrawText(text.c_str(), textX, textY, fontSize, BLACK);

    if (bMousePressed)
    {
        return bHover;
    }
    return false;
}

void BlueprintGUI::DrawNode(const Node& node, int fontSize)
{
    int textWidth = MeasureText(node.Name.c_str(), fontSize);

    int CenterX = node.PosX * c_StepSizeX;
    int CenterY = node.PosY * c_StepSizeY;
    const int Padding = 20;
    Rectangle rect(CenterX - (textWidth + Padding) / 2, CenterY - (fontSize + Padding) / 2, textWidth + Padding, fontSize + Padding);

    DrawRectangleRounded(rect, 0.2, 10, GRAY);
    DrawText(node.Name.c_str(), CenterX - textWidth / 2, CenterY - fontSize / 2, fontSize, WHITE);
}

void BlueprintGUI::OpenFile()
{
    nfdu8char_t* outPath = nullptr;
    nfdu8filteritem_t filters[] = {
        { "Graph data", "yaml" }
    };

    nfdresult_t result = NFD_OpenDialogU8(&outPath, filters, 1, nullptr);

    if (result == NFD_OKAY) 
    {
        m_AutoLayout.ImportGraph(outPath);
        m_AutoLayout.CreateLayout();
        NFD_FreePathU8(outPath);
    }
}

void BlueprintGUI::SaveFile()
{
    nfdu8char_t* outPath = nullptr;
    nfdu8filteritem_t filters[] = {
        { "Graph data", "yaml" }
    };
    nfdresult_t result = NFD_SaveDialogU8(&outPath, filters, 1, nullptr, "layout.yaml");

    if (result == NFD_OKAY)
    {
        std::cout << outPath << std::endl;
        m_AutoLayout.ExportGraph(outPath);
        NFD_FreePathU8(outPath);
    }
}
