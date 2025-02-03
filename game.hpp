#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <dwmapi.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")
IDirect3D9Ex* p_object = NULL;
IDirect3DDevice9Ex* p_device = NULL;
D3DPRESENT_PARAMETERS p_params = { NULL };
MSG messager = { NULL };
HWND my_wnd = NULL;
HWND game_wnd = NULL;
DWORD processID;
HRESULT directx_init()
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_object))) exit(3);
	ZeroMemory(&p_params, sizeof(p_params));
	p_params.Windowed = TRUE;
	p_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_params.hDeviceWindow = my_wnd;
	p_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_params.BackBufferWidth = settings::width;
	p_params.BackBufferHeight = settings::height;
	p_params.EnableAutoDepthStencil = TRUE;
	p_params.AutoDepthStencilFormat = D3DFMT_D16;
	p_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	if (FAILED(p_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, my_wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_params, 0, &p_device)))
	{
		p_object->Release();
		exit(4);
	}
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(my_wnd);
	ImGui_ImplDX9_Init(p_device);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = 0;
	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowBorderSize = 1;
	style->FrameBorderSize = 1;
	style->WindowTitleAlign = { 0.5f, 0.5f };
	style->Colors[ImGuiCol_BorderShadow] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_Border] = ImColor(8, 8, 8, 250);
	style->Colors[ImGuiCol_TitleBg] = ImColor(38, 38, 38, 250);
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(38, 38, 38, 250);
	style->Colors[ImGuiCol_WindowBg] = ImColor(38, 38, 38, 250);
	style->Colors[ImGuiCol_FrameBg] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_Button] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(38, 38, 38, 245);
	style->Colors[ImGuiCol_SliderGrab] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_SliderGrabActive] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_CheckMark] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_Header] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_HeaderActive] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_HeaderHovered] = ImColor(68, 68, 68, 250);
	style->Colors[ImGuiCol_PopupBg] = ImColor(38, 38, 38, 250);
	p_object->Release();
	return S_OK;
}

void create_overlay()
{
	WNDCLASSEXA wcex = {
		sizeof(WNDCLASSEXA),
		0,
		DefWindowProcA,
		0,
		0,
		0,
		LoadIcon(0, IDI_APPLICATION),
		LoadCursor(0, IDC_ARROW),
		0,
		0,
		"SofMainFNBase",
		LoadIcon(0, IDI_APPLICATION)
	};
	ATOM rce = RegisterClassExA(&wcex);
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	my_wnd = gui::create_window_in_band(0, rce, L"SofMainFNBase", WS_POPUP, rect.left, rect.top, rect.right, rect.bottom, 0, 0, wcex.hInstance, 0, gui::ZBID_UIACCESS);
	SetWindowLong(my_wnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(my_wnd, &margin);
	ShowWindow(my_wnd, SW_SHOW);
	UpdateWindow(my_wnd);
}

void aimbot(uintptr_t target_mesh)
{
	if (!target_mesh) return;
	if (!is_visible(target_mesh)) return;
	Vector3 head3d = get_entity_bone(target_mesh, 110);
	Vector2 head2d = project_world_to_screen(head3d);
	Vector2 target{};
	if (head2d.x != 0)
	{
		if (head2d.x > settings::screen_center_x)
		{
			target.x = -(settings::screen_center_x - head2d.x);
			target.x /= settings::aimbot::smoothness;
			if (target.x + settings::screen_center_x > settings::screen_center_x * 2) target.x = 0;
		}
		if (head2d.x < settings::screen_center_x)
		{
			target.x = head2d.x - settings::screen_center_x;
			target.x /= settings::aimbot::smoothness;
			if (target.x + settings::screen_center_x < 0) target.x = 0;
		}
	}
	if (head2d.y != 0)
	{
		if (head2d.y > settings::screen_center_y)
		{
			target.y = -(settings::screen_center_y - head2d.y);
			target.y /= settings::aimbot::smoothness;
			if (target.y + settings::screen_center_y > settings::screen_center_y * 2) target.y = 0;
		}
		if (head2d.y < settings::screen_center_y)
		{
			target.y = head2d.y - settings::screen_center_y;
			target.y /= settings::aimbot::smoothness;
			if (target.y + settings::screen_center_y < 0) target.y = 0;
		}
	}
	input::move_mouse(target.x, target.y);
}

void draw_cornered_box(int x, int y, int w, int h, const ImColor color, int thickness)
{
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), color, thickness);
}



void Box(int X, int Y, int W, int H, const ImColor color, int thickness)
{
	ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)Y }, ImVec2{ (float)(X + W), (float)Y }, color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)(X + W), (float)Y }, ImVec2{ (float)(X + W), (float)(Y + H) }, color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)(Y + H) }, ImVec2{ (float)(X + W), (float)(Y + H) }, color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)Y }, ImVec2{ (float)X, (float)(Y + H) }, color, thickness);
}

void draw_filled_rect(int x, int y, int w, int h, const ImColor color)
{
	ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}

void draw_line(Vector2 target, const ImColor color)
{
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(settings::screen_center_x, settings::height), ImVec2(target.x, target.y), color, 0.1f);
}

void draw_distance(Vector2 location, float distance, const ImColor color)
{
	char dist[64];
	sprintf_s(dist, "%.fm", distance);
	ImVec2 text_size = ImGui::CalcTextSize(dist);
	ImGui::GetForegroundDrawList()->AddText(ImVec2(location.x - text_size.x / 2, location.y - text_size.y / 2), color, dist);
}

#include <chrono>

using namespace std::chrono;

void Debugg()
{
	// Get the current time to calculate FPS
	static auto lastTime = high_resolution_clock::now();
	static int frameCount = 0;

	// Calculate FPS
	auto currentTime = high_resolution_clock::now();
	duration<float> deltaTime = currentTime - lastTime;
	lastTime = currentTime;
	frameCount++;

	// Update FPS every second
	static float fps = 0.0f;
	if (deltaTime.count() >= 1.0f) {
		fps = frameCount / deltaTime.count();
		frameCount = 0;
	}

	// Set watermark text
	const char* text = "[Status: Undedected] - [Developer: SofMain]";


	// Set FPS text
	char fpsText[64];
	snprintf(fpsText, sizeof(fpsText), "FPS: %.2f", fps);

	// Combine watermark text and FPS
	const char* fullText = nullptr;
	char combinedText[256];
	snprintf(combinedText, sizeof(combinedText), "%s | %s", text, fpsText);
	fullText = combinedText;

	// Calculate text size and position
	ImVec2 position = ImVec2(30, 30); // Starting position of the watermark
	ImVec2 textSize = ImGui::CalcTextSize(fullText);

	// Adjust position based on the text size
	ImVec2 newPosition = ImVec2(position.x, position.y + textSize.y);

	// Draw the watermark with FPS on the screen
	ImGui::SetCursorPos(newPosition);
	ImGui::Text("%s", fullText);
}

ImVec2 GetWatermarkSize()
{
	ImVec2 position = ImVec2(30, 30);
	const char* text = "SofMain FN Base [Private Build]";

	//ImVec2 position = ImVec2(30, 30);
	//const char* text = "Fortnite [Public]";

	ImVec2 textSize = ImGui::CalcTextSize(text);
	return ImVec2(position.x, position.y + textSize.y);
}



void game_loop()
{
	cache::uworld = read<uintptr_t>(Base + UWORLD);
	cache::game_instance = read<uintptr_t>(cache::uworld + GAME_INSTANCE);
	cache::local_players = read<uintptr_t>(read<uintptr_t>(cache::game_instance + LOCAL_PLAYERS));
	cache::player_controller = read<uintptr_t>(cache::local_players + PLAYER_CONTROLLER);
	cache::local_pawn = read<uintptr_t>(cache::player_controller + LOCAL_PAWN);
	if (cache::local_pawn != 0)
	{
		cache::root_component = read<uintptr_t>(cache::local_pawn + ROOT_COMPONENT);
		cache::relative_location = read<Vector3>(cache::root_component + RELATIVE_LOCATION);
		cache::player_state = read<uintptr_t>(cache::local_pawn + PLAYER_STATE);
		cache::my_team_id = read<int>(cache::player_state + TEAM_INDEX);
	}
	cache::game_state = read<uintptr_t>(cache::uworld + GAME_STATE);
	cache::player_array = read<uintptr_t>(cache::game_state + PLAYER_ARRAY);
	cache::player_count = read<int>(cache::game_state + (PLAYER_ARRAY + sizeof(uintptr_t)));
	cache::closest_distance = FLT_MAX;
	cache::closest_mesh = NULL;
	for (int i = 0; i < cache::player_count; i++)
	{
		uintptr_t player_state = read<uintptr_t>(cache::player_array + (i * sizeof(uintptr_t)));
		if (!player_state) continue;
		int player_team_id = read<int>(player_state + TEAM_INDEX);
		if (player_team_id == cache::my_team_id) continue;
		uintptr_t pawn_private = read<uintptr_t>(player_state + PAWN_PRIVATE);
		if (!pawn_private) continue;
		if (pawn_private == cache::local_pawn) continue;
		uintptr_t mesh = read<uintptr_t>(pawn_private + MESH);
		if (!mesh) continue;
		Vector3 head3d = get_entity_bone(mesh, 110);
		Vector2 head2d = project_world_to_screen(head3d);
		Vector3 bottom3d = get_entity_bone(mesh, 0);
		Vector2 bottom2d = project_world_to_screen(bottom3d);
		float box_height = abs(head2d.y - bottom2d.y);
		float box_width = box_height * 0.50f;
		float distance = cache::relative_location.distance(bottom3d) / 100;
		if (settings::visuals::enable)
		{
			if (settings::visuals::Box)
			{
				if (is_visible(mesh))
				{
					Box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(250, 250, 250, 250), 1);
				}
				else
				{
					Box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(250, 0, 0, 250), 1);
				}
				if (settings::visuals::fill_box) draw_filled_rect(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(0, 0, 0, 50));
			}
		}
		if (settings::visuals::enable)
		{
			if (settings::visuals::CorneredBox)
			{
				if (is_visible(mesh))
				{
					draw_cornered_box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(0, 0, 250, 250), 1);
				}
				else
				{
					draw_cornered_box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(0, 0, 255, 250), 1);
				}
				if (settings::visuals::fill_box) draw_filled_rect(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(0, 0, 0, 50));
			}
			
			if (settings::visuals::skeleton)
			{
				Vector2 bonePositions[16];
				const int boneIndices[] = { 110, 3, 66, 9, 38, 10, 39, 11, 40, 78, 71, 79, 72, 75, 82, 67 };
				for (int i = 0; i < 16; ++i)
				{
					bonePositions[i] = project_world_to_screen(get_entity_bone(mesh, boneIndices[i]));
				}
				std::pair<int, int> bonePairs[] =
				{
					{1, 2}, {3, 4}, {4, 3}, {5, 3}, {6, 4}, {5, 7}, {6, 8},
					{10, 1}, {9, 1}, {12, 10}, {11, 9}, {13, 12}, {14, 11}, {2, 15}, { 15, 0 }
				};

				ImGui::GetBackgroundDrawList()->PushClipRectFullScreen();
				for (const auto& pair : bonePairs)
				{
					ImVec2 start(bonePositions[pair.first].x, bonePositions[pair.first].y);
					ImVec2 end(bonePositions[pair.second].x, bonePositions[pair.second].y);

					if (pair.first == 110)
					{
						end.y += 20;
					}

					if (is_visible(mesh))
					{
						ImGui::GetBackgroundDrawList()->AddLine(start, end, ImColor(255, 0, 255, 250), 1);
					}
					else
					{
						ImGui::GetBackgroundDrawList()->AddLine(start, end, ImColor(255, 165, 0, 250), 1);
					}

				}
				ImGui::GetBackgroundDrawList()->PopClipRect();
			}
			
			if (settings::visuals::line)
			{
				if (is_visible(mesh))
				{
					draw_line(bottom2d, ImColor(0, 0, 250, 250));
				}
				else
				{
					draw_line(bottom2d, ImColor(0, 0, 255, 250));
				}
			}
			if (settings::visuals::distance)
			{
				draw_distance(bottom2d, distance, ImColor(250, 250, 250, 250));
			}
		}
		double dx = head2d.x - settings::screen_center_x;
		double dy = head2d.y - settings::screen_center_y;
		float dist = sqrtf(dx * dx + dy * dy);
		if (dist <= settings::aimbot::fov && dist < cache::closest_distance)
		{
			cache::closest_distance = dist;
			cache::closest_mesh = mesh;
		}
	}
	if (settings::aimbot::enable)
	{
		if (GetAsyncKeyState(settings::aimbot::current_key)) aimbot(cache::closest_mesh);
	}
	if (crosshair::Crosshair)
	{
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();
		ImVec2 screen_size = ImGui::GetIO().DisplaySize;
		ImVec2 center = ImVec2(screen_size.x / 2, screen_size.y / 2);

		// Ensure crosshair length is set (you may want to define this in settings as well)
		float crosshair_length = 7.0f;  // Set a default value if not set

		// Convert the stored crosshair color (U32) to ImColor for drawing
		ImColor crosshair_color = ImColor(crosshair::crosshair_color);

		// Calculate start and end points for horizontal and vertical lines
		ImVec2 horizontal_start = ImVec2(center.x - crosshair_length, center.y);
		ImVec2 horizontal_end = ImVec2(center.x + crosshair_length, center.y);

		ImVec2 vertical_start = ImVec2(center.x, center.y - crosshair_length);
		ImVec2 vertical_end = ImVec2(center.x, center.y + crosshair_length);

		// Draw the horizontal and vertical lines for the crosshair
		draw_list->AddLine(horizontal_start, horizontal_end, crosshair_color, crosshair::crosshair_thickness);
		draw_list->AddLine(vertical_start, vertical_end, crosshair_color, crosshair::crosshair_thickness);
	}
	if (settings::Features::Watermark) {

		static ImVec4 colorArray[6] = {
			ImVec4(0.63f, 0.13f, 0.94f, 1.0f),  // Purple
			ImVec4(0.0f, 0.0f, 1.0f, 1.0f),     // Blue
			ImVec4(1.0f, 0.0f, 0.0f, 1.0f),     // Red
			ImVec4(1.0f, 0.0f, 1.0f, 1.0f),     // Magenta
			ImVec4(0.0f, 1.0f, 1.0f, 1.0f),     // Baby Blue
			ImVec4(0.0f, 1.0f, 0.0f, 1.0f),     // Green
		};


		static int currentColorIndex = 0;    // Current starting color index
		static int nextColorIndex = 1;       // Next color index
		static float fadeProgress = 0.0f;    // Progress of the fade (0.0 to 1.0)


		fadeProgress += ImGui::GetIO().DeltaTime * 0.5f; // Adjust multiplier to change fade speed (0.5f = slower)


		if (fadeProgress >= 1.0f) {
			fadeProgress = 0.0f;                          // Reset fade progress
			currentColorIndex = nextColorIndex;          // Update the current color
			nextColorIndex = (nextColorIndex + 1) % 6;   // Move to the next color in the array
		}


		ImVec4 currentColor = colorArray[currentColorIndex];
		ImVec4 nextColor = colorArray[nextColorIndex];
		ImVec4 interpolatedColor = ImVec4(
			currentColor.x + (nextColor.x - currentColor.x) * fadeProgress,
			currentColor.y + (nextColor.y - currentColor.y) * fadeProgress,
			currentColor.z + (nextColor.z - currentColor.z) * fadeProgress,
			currentColor.w + (nextColor.w - currentColor.w) * fadeProgress
		);


		ImVec2 position = ImVec2(30, 30);
		const char* text = "Sofmain rework by landen1337";


		ImVec2 textSize = ImGui::CalcTextSize(text);


		float paddingX = 10.0f;
		float paddingY = 5.0f;


		ImColor boxColor = ImColor(0.f, 0.f, 0.f, 0.7f);
		ImVec2 boxMin = ImVec2(position.x - paddingX, position.y - paddingY);
		ImVec2 boxMax = ImVec2(position.x + textSize.x + paddingX, position.y + textSize.y + paddingY);

		// Draw the box behind the text
		ImGui::GetForegroundDrawList()->AddRectFilled(boxMin, boxMax, boxColor);


		ImGui::GetForegroundDrawList()->AddText(position, ImColor(interpolatedColor), text);
	}
	if (settings::Features::Debug) {


		Debugg();

	}
}


#include <vector>
#include <random>

struct Snowflake {
	float x, y;         // Position of the snowflake
	float speed;        // Speed of the snowflake falling
	float size;         // Size of the snowflake
};




void render_snowflakes(bool menuVisible)
{
	static std::vector<Snowflake> snowflakes;
	static std::default_random_engine generator;
	static std::uniform_real_distribution<float> distributionX(0.0f, ImGui::GetIO().DisplaySize.x);
	static std::uniform_real_distribution<float> distributionSize(2.0f, 5.0f);
	static std::uniform_real_distribution<float> distributionSpeed(0.1f, 1.0f);

	if (menuVisible) {

		if (snowflakes.size() < 200) {
			snowflakes.push_back(Snowflake{
				distributionX(generator),
				0.0f,
				distributionSpeed(generator),
				distributionSize(generator)
				});
		}


		for (auto& snowflake : snowflakes) {
			snowflake.y += snowflake.speed;
			if (snowflake.y > ImGui::GetIO().DisplaySize.y) {
				snowflake.y = 0.0f;
			}

			// Draw snowflake as a small circle
			ImGui::GetForegroundDrawList()->AddCircleFilled(
				ImVec2(snowflake.x, snowflake.y),
				snowflake.size,
				IM_COL32(255, 255, 255, 255)
			);
		}
	}
	else {

		snowflakes.clear();
	}
}

void DrawCustomCursor() {
	// Get the current mouse position
	ImVec2 mouse_pos = ImGui::GetMousePos();

	// Get the foreground draw list to draw on top of everything
	ImDrawList* draw_list = ImGui::GetForegroundDrawList();

	// Define the cursor style (e.g., a small circle)
	float radius = 5.0f;                          // Cursor radius
	ImU32 color = IM_COL32(255, 0, 247, 255);   // White color
	ImU32 border_color = IM_COL32(0, 0, 0, 255);  // Black border

	// Draw the circle cursor
	draw_list->AddCircleFilled(mouse_pos, radius, color);           // Inner circle
	draw_list->AddCircle(mouse_pos, radius + 1.0f, border_color);   // Outer border
}

void Crosshair()
{
	if (ImGui::Checkbox("Crosshair", &settings::Crosshair))
	{
		//ImVec4 color = ImGui::ColorConvertU32ToFloat4(settings::crosshair_color);
		//if (ImGui::ColorEdit4("Crosshair Color", (float*)&color))
	//	{
			//settings::crosshair_color = ImGui::ColorConvertFloat4ToU32(color);
	//	}
		ImGui::SliderFloat("Thickness", &settings::crosshair_thickness, 1.0f, 10.0f);

		if (settings::Crosshair) // Only show the combo box if the crosshair is enabled
		{

		}
	}

}


void render_menu()
{
	
		static float colorTimer = 0.0f;
		static int currentColorIndex = 0;
		static int nextColorIndex = 1;


		ImVec4 colorArray[6] = {
			ImVec4(0.63f, 0.13f, 0.94f, 1.0f), // Purple
			ImVec4(0.0f, 0.0f, 1.0f, 1.0f),    // Blue
			ImVec4(1.0f, 0.0f, 0.0f, 1.0f),    // Red
			ImVec4(1.0f, 0.0f, 1.0f, 1.0f),    // Magenta
			ImVec4(0.0f, 1.0f, 1.0f, 1.0f),    // Baby Blue
			ImVec4(0.0f, 1.0f, 0.0f, 1.0f),    // Green
		};


		colorTimer += ImGui::GetIO().DeltaTime * 0.5f;


		if (colorTimer >= 1.0f)
		{
			colorTimer = 0.0f;
			currentColorIndex = nextColorIndex;
			nextColorIndex = (nextColorIndex + 1) % 6; // cycle to the next color/ yu can change if needed
		}

		// Interpolate between current and next color
		ImVec4 currentColor = colorArray[currentColorIndex];
		ImVec4 nextColor = colorArray[nextColorIndex];
		ImVec4 interpolatedColor = ImVec4(
			currentColor.x + (nextColor.x - currentColor.x) * colorTimer,
			currentColor.y + (nextColor.y - currentColor.y) * colorTimer,
			currentColor.z + (nextColor.z - currentColor.z) * colorTimer,
			currentColor.w + (nextColor.w - currentColor.w) * colorTimer
		);
		
		render_snowflakes(settings::show_menu);
#ifndef IM_PI
#define IM_PI 3.14159265358979323846f
#endif

	switch (settings::aimbot::current_aimkey)
	{
	case 0:
		settings::aimbot::current_key = VK_LBUTTON;
	case 1:
		settings::aimbot::current_key = VK_RBUTTON;
	}
	if (settings::aimbot::show_fov) {
		// Predefined set of fixed colors
		static std::vector<ImColor> colors = {
			ImColor(255, 0, 0, 250),   // Red
			ImColor(0, 255, 0, 250),   // Green
			ImColor(0, 0, 255, 250),   // Blue
			ImColor(255, 255, 0, 250)  // Yellow
		};

		static int colorIndex = 0;          // Current color index
		static int nextColorIndex = 1;      // Next target color index
		static float transitionProgress = 0.0f;  // Transition progress (0 to 1)

		// Get current and next colors
		ImColor currentColor = colors[colorIndex];
		ImColor nextColor = colors[nextColorIndex];

		// Interpolate between colors
		ImVec4 currentVec = currentColor.Value;
		ImVec4 nextVec = nextColor.Value;

		// LERP between current and next color
		ImVec4 blendedColor = ImVec4(
			currentVec.x + (nextVec.x - currentVec.x) * transitionProgress,
			currentVec.y + (nextVec.y - currentVec.y) * transitionProgress,
			currentVec.z + (nextVec.z - currentVec.z) * transitionProgress,
			currentVec.w + (nextVec.w - currentVec.w) * transitionProgress
		);

		// Draw the FOV circle with the blended color
		ImGui::GetForegroundDrawList()->AddCircle(
			ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2),
			settings::aimbot::fov,
			ImGui::ColorConvertFloat4ToU32(blendedColor),
			100,
			1.0f
		);

		// Increment transition progress over time
		transitionProgress += ImGui::GetIO().DeltaTime * 1.5f;  // Adjust speed here

		// When transition completes, move to the next color
		if (transitionProgress >= 1.0f) {
			transitionProgress = 0.0f;
			colorIndex = nextColorIndex;
			nextColorIndex = (nextColorIndex + 1) % colors.size();
		}
	}

	if (GetAsyncKeyState(VK_INSERT) & 1) settings::show_menu = !settings::show_menu;
	if (settings::show_menu)
	{
		ImGui::SetNextWindowSize({ 550, 350 });
		ImGui::Begin("Sofmain rework by landen1337", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 10.0f;
		style.WindowRounding = 10.0f;
		style.ScrollbarRounding = 10.0f;
		style.Colors[ImGuiCol_Border] = interpolatedColor;
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.50f, 0.80f, 1.0f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.10f, 0.40f, 0.60f, 1.0f);
		ImGui::BeginChild("Tabs", ImVec2(150, 0), true);
		const char* tabs[] = { "Aimbot", "Visuals", "Links", "Exploits", "Exit" };
		for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
		{
			if (ImGui::Selectable(tabs[i], settings::tab == i))
				settings::tab = i;
			ImGui::Spacing();
		}
		ImGui::EndChild();

		ImGui::SameLine();
		ImGui::BeginChild("MainContent", ImVec2(0, 0), true);


		switch (settings::tab)
		{
		case 0:
		{
			ImGui::Checkbox("Enable", &settings::aimbot::enable);
			ImGui::Checkbox("RGB Fov", &settings::aimbot::show_fov);
			ImGui::SliderFloat("##Fov", &settings::aimbot::fov, 50, 300, "Fov: %.2f");
			ImGui::SliderFloat("##Smoothness", &settings::aimbot::smoothness, 1, 10, "Smoothness: %.2f");
			ImGui::Combo("##Aimkey", &settings::aimbot::current_aimkey, settings::aimbot::aimkey, sizeof(settings::aimbot::aimkey) / sizeof(*settings::aimbot::aimkey));
			break;
		}
		case 1:
		{
			ImGui::Checkbox("Enable", &settings::visuals::enable);
			ImGui::Checkbox("Skeleton", &settings::visuals::skeleton);
			ImGui::Checkbox("Cornered Box", &settings::visuals::CorneredBox);
			ImGui::SameLine();
			ImGui::Checkbox("Fill Box", &settings::visuals::fill_box);
			ImGui::Checkbox("Box", &settings::visuals::Box);
			ImGui::Checkbox("Line", &settings::visuals::line);
			ImGui::Checkbox("Distance", &settings::visuals::distance);
			ImGui::Checkbox("Crosshair", &crosshair::Crosshair);
			if (crosshair::Crosshair)
			{
				// Convert crosshair color from U32 to ImVec4 format for easy manipulation
				ImVec4 color = ImGui::ColorConvertU32ToFloat4(crosshair::crosshair_color);

				// Create a color menu for adjusting the crosshair color
				if (ImGui::BeginCombo("Select Crosshair Color", "Select Color"))
				{
					if (ImGui::ColorEdit4("Custom Color", (float*)&color, ImGuiColorEditFlags_AlphaPreviewHalf))
					{
						// After the color is modified, convert it back to U32 and store it
						crosshair::crosshair_color = ImGui::ColorConvertFloat4ToU32(color);
					}

					// You can also provide preset color options
					if (ImGui::Selectable("Red"))
					{
						color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
						crosshair::crosshair_color = ImGui::ColorConvertFloat4ToU32(color);
					}
					if (ImGui::Selectable("Green"))
					{
						color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
						crosshair::crosshair_color = ImGui::ColorConvertFloat4ToU32(color);
					}
					if (ImGui::Selectable("Blue"))
					{
						color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
						crosshair::crosshair_color = ImGui::ColorConvertFloat4ToU32(color);
					}
					if (ImGui::Selectable("White"))
					{
						color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
						crosshair::crosshair_color = ImGui::ColorConvertFloat4ToU32(color);
					}
					if (ImGui::Selectable("Yellow"))
					{
						color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
						crosshair::crosshair_color = ImGui::ColorConvertFloat4ToU32(color);
					}

					// End the color selection menu
					ImGui::EndCombo();
				}

				// Slider for adjusting the thickness of the crosshair
				ImGui::SliderFloat("Thickness", &crosshair::crosshair_thickness, 1.0f, 10.0f);
			}

			break;
		}
		case 2:
		{
			if (ImGui::Button("Sofmains Discord", { 130, 30 }))
			{

				ShellExecuteA(0, "open", "https://discord.gg/vz9CpEcNED", 0, 0, SW_SHOWNORMAL);
			}
			if (ImGui::Button("Sofmains Github", { 130, 30 }))
			{

				ShellExecuteA(0, "open", "https://github.com/sofmain-1337", 0, 0, SW_SHOWNORMAL);
			}
			if (ImGui::Button("My Discord", { 130, 30 }))
			{

				ShellExecuteA(0, "open", "https://discord.gg/aethercheats", 0, 0, SW_SHOWNORMAL);
			}
			if (ImGui::Button("My Github", { 130, 30 }))
			{

				ShellExecuteA(0, "open", "https://github.com/Pearlism", 0, 0, SW_SHOWNORMAL);
			}


			ImVec2 discordButtonPos = ImGui::GetItemRectMin();
			float buttonHeight = 30.0f;

			ImVec2 textPosition = ImVec2(discordButtonPos.x, discordButtonPos.y + buttonHeight + 5);

			ImVec4 messageColor = ImVec4(0.63f, 0.13f, 0.94f, 1.0f);


			break;
		}

		case 3:
		{
			ImGui::Checkbox("Aim in air", &settings::Exploits::AimInAir);
			ImGui::Checkbox("No Recoil", &settings::Exploits::no_recoil);

			ImVec4 messageColor = ImVec4(0.63f, 0.13f, 0.94f, 1.0f);

			ImGui::SetCursorPos(ImVec2(20, 50));
			ImGui::TextColored(messageColor, "Exploits don't have functions");
			//heres your exploits via case3
			ImGui::SetCursorPos(ImVec2(20, 70));
			ImGui::TextColored(messageColor, "so you have to add them to work");

			ImGui::SetCursorPos(ImVec2(20, 90));
			ImGui::TextColored(messageColor, "Landen419/landen1337 loves you");

			break;
		}

		case 4:
		{
			if (ImGui::Button("Exit Cheat", { 130, 30 })) exit(0);
		}
		}

		ImGui::EndChild();

		ImGui::End();
	}
}

HWND get_process_wnd(uint32_t pid)
{
	std::pair<HWND, uint32_t> params = { 0, pid };
	BOOL bresult = EnumWindows([](HWND hwnd, LPARAM lparam) -> BOOL
	{
		auto pparams = (std::pair<HWND, uint32_t>*)(lparam);
		uint32_t processid = 0;
		if (GetWindowThreadProcessId(hwnd, reinterpret_cast<LPDWORD>(&processid)) && processid == pparams->second)
		{
			SetLastError((uint32_t)-1);
			pparams->first = hwnd;
			return FALSE;
		}
		return TRUE;
	}, (LPARAM)&params);
	if (!bresult && GetLastError() == -1 && params.first) return params.first;
	return 0;
}

WPARAM render_loop()
{
	static RECT old_rc;
	ZeroMemory(&messager, sizeof(MSG));
	while (messager.message != WM_QUIT)
	{
		if (PeekMessage(&messager, my_wnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&messager);
			DispatchMessage(&messager);
		}
		if (game_wnd == NULL) exit(0);
		HWND active_wnd = GetForegroundWindow();
		if (active_wnd == game_wnd)
		{
			HWND target_wnd = GetWindow(active_wnd, GW_HWNDPREV);
			SetWindowPos(my_wnd, target_wnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		else
		{
			game_wnd = get_process_wnd(processID);
			Sleep(250);
		}
		RECT rc;
		POINT xy;
		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		rc.left = xy.x;
		rc.top = xy.y;
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;
		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;
		if (GetAsyncKeyState(0x1))
		{
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
		{
			io.MouseDown[0] = false;
		}
		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{
			old_rc = rc;
			settings::width = rc.right;
			settings::height = rc.bottom;
			p_params.BackBufferWidth = settings::width;
			p_params.BackBufferHeight = settings::height;
			SetWindowPos(my_wnd, (HWND)0, xy.x, xy.y, settings::width, settings::height, SWP_NOREDRAW);
			p_device->Reset(&p_params);
		}
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		game_loop();
		render_menu();
		ImGui::EndFrame();
		p_device->SetRenderState(D3DRS_ZENABLE, false);
		p_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		p_device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		p_device->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
		if (p_device->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			p_device->EndScene();
		}
		HRESULT result = p_device->Present(0, 0, 0, 0);
		if (result == D3DERR_DEVICELOST && p_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_device->Reset(&p_params);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (p_device != 0)
	{
		p_device->EndScene();
		p_device->Release();
	}
	if (p_object != 0) p_object->Release();
	DestroyWindow(my_wnd);
	return messager.wParam;
}
