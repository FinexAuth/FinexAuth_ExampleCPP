#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_freetype.h"
#include <imgui_internal.h>


#include "imgui_impl_dx11.h"

#include <d3d11.h>
#include <tchar.h>

#include <D3DX11tex.h>
#pragma comment(lib, "D3DX11.lib")
#include <thread>
#include <string>
#include <vector>


#pragma comment(lib, "Dwmapi.lib")
#include <dwmapi.h>

#include "imgui_impl_dx11.h"
#include "ImGui_Settings.h"
#include <d3d11.h>
#include <dwmapi.h>
#include <tchar.h>
#include <thread>
#include <string>
#include <vector>






// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


HWND hwnd;
RECT rc;
DWORD window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove;


void move_window() {
    ImGui::SetCursorPos(ImVec2(0, 0));
    if (ImGui::InvisibleButton("Move_detector", ImVec2(c::bg::size.x, c::bg::size.y)));
    if (ImGui::IsItemActive()) {

        GetWindowRect(hwnd, &rc);
        MoveWindow(hwnd, rc.left + ImGui::GetMouseDragDelta().x, rc.top + ImGui::GetMouseDragDelta().y, c::bg::size.x, c::bg::size.y, TRUE);
    }
}


void move_window2() {
    ImGui::SetCursorPos(ImVec2(0, 0));
    if (ImGui::InvisibleButton("Move_detector", ImVec2(c::bg::size2.x, c::bg::size2.y)));
    if (ImGui::IsItemActive()) {

        GetWindowRect(hwnd, &rc);
        MoveWindow(hwnd, rc.left + ImGui::GetMouseDragDelta().x, rc.top + ImGui::GetMouseDragDelta().y, c::bg::size2.x, c::bg::size2.y, TRUE);
    }
}
