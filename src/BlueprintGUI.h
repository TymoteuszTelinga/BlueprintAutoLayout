#pragma once

#include <raylib.h>

#include "BlueprintLayout.h"

class BlueprintGUI
{
public:
	BlueprintGUI();
	~BlueprintGUI();

	void Run();
	void OnUpdate(float deltaTime);
	void OnRender();

private:
	bool Button(const Rectangle& rect, const std::string& text, uint32_t fontSize);
	void DrawNode(const Node& node, int fontSize);

	void OpenFile();
	void SaveFile();

private:
	BlueprintAutoLayout m_AutoLayout;

	Camera2D m_Camera;
	Vector2 m_CameraPos;

	Vector2 m_MousePosition;
	bool bMousePressed = false;

	const uint32_t c_WindowWidth = 1280;
	const uint32_t c_WindowHeight = 720;
	const float c_CameraSpeed = 200.0f;
	const uint32_t c_StepSizeX = 300;
	const uint32_t c_StepSizeY = 150;
};