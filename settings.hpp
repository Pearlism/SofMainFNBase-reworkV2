#include "imgui/imgui.h"
//SIMPLE ASS SETTINGS HERE
#pragma once
namespace crosshair
{
	float crosshair_thickness = 2.0f;
	float crosshair_radius = 3.0f;
	float crosshair_length = 10.0f;
	float crosshair_color = IM_COL32(7, 255, 255, 255);
	inline bool Crosshair = true;
}
namespace settings
{
	inline int width = GetSystemMetrics(SM_CXSCREEN);
	inline int height = GetSystemMetrics(SM_CYSCREEN);
	inline int screen_center_x = width / 2;
	inline int screen_center_y = height / 2;
	inline bool show_menu = true;
	inline int tab = 0;
	namespace aimbot
	{
		inline bool enable = false;
		inline bool show_fov = false;
		inline float fov = 150;
		inline float smoothness = 5;
		inline static const char* aimkey[] = { "Left Mouse Button", "Right Mouse Button" };
		inline static int current_aimkey = 1;
		inline static int current_key = VK_RBUTTON;
	}
	namespace visuals
	{
		inline bool enable = false;
		inline bool CorneredBox = false;
		inline bool fill_box = false;
		inline bool line = false;
		inline bool distance = false;
		inline bool Box;
		inline bool skeleton = true;
	}
	namespace Features
	{
		inline bool FPS = true;
		bool isVSyncEnabled = true;
		inline bool Watermark = true;
		inline bool RenderCount;
		inline bool Debug = false;
	}
	namespace Exploits
	{
		inline bool AimInAir = false;
		inline bool no_recoil = false;
	}
}
