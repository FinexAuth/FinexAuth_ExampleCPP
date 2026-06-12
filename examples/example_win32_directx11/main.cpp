#include "main.h"
#include "FinexAuth//auth.hpp"
#include "FinexAuth/skStr.h"


using namespace FNX;

// Base config
std::string name = skCrypt("").decrypt();//Application Name (doesn't have to be the same as your app, but must match the name in your dashboard)
std::string ownerid = skCrypt("").decrypt();//Mester Secret Key Is A ownerd Id (found on your dashboard, click your profile to find it)
std::string version = skCrypt("1.0").decrypt();// Must Be The Same As Your Version On Dashboard
std::string url = skCrypt("https://finex-authenticator.pages.dev/").decrypt();


// Forward declare the global API instance
api FNXApp(name, ownerid, version, url);




char search[24] = {0};
namespace font
{
    ImFont* icomoon[20]      = {nullptr};
    ImFont* segue_semibold   = nullptr;
}

static bool        Login_mneu   = true;
static bool        Main_Menu = false;
static std::string g_logged_in_name = "";
static int         g_active_tab     = 0; 

static char          username[128]       = "";
static char          password[128]       = "";
static char          license_key[128]    = "";
static char          reg_error[128] = "";

static void PushInputStyle()
{
    ImGui::PushStyleColor(ImGuiCol_FrameBg,        ImColor(36,  40,  50,  255).Value);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor(46,  52,  65,  255).Value);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  ImColor(56,  62,  78,  255).Value);
    ImGui::PushStyleColor(ImGuiCol_Text,           ImColor(220, 228, 240, 255).Value);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,  ImVec2(10.f, 8.f));
}
static void PopInputStyle()
{
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(4);
}

static void PushAccentButton()
{
    ImGui::PushStyleColor(ImGuiCol_Button,        ImColor(0,   200, 210, 255).Value);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(0,   220, 235, 255).Value);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImColor(0,   170, 180, 255).Value);
    ImGui::PushStyleColor(ImGuiCol_Text,          ImColor(10,  16,  20,  255).Value);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
}
static void PopAccentButton()
{
    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(4);
}

static void PushDisabledButton()
{
    ImGui::PushStyleColor(ImGuiCol_Button,        ImColor(30, 38, 50, 200).Value);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(30, 38, 50, 200).Value);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImColor(30, 38, 50, 200).Value);
    ImGui::PushStyleColor(ImGuiCol_Text,          ImColor(70, 90, 120, 255).Value);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
}
static void PopDisabledButton()
{
    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(4);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    
   

    FNXApp.init();// Initialize the API (this doesn't log in, it just sets up the instance ready for login)
  

    WNDCLASSEXW wc   = {};
    wc.cbSize        = sizeof(WNDCLASSEXW);
    wc.style         = CS_CLASSDC;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(0, IDC_ARROW);
    wc.lpszMenuName  = L"FinexAuth";
    wc.lpszClassName = L"FinexAuth";
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(
        ::MonitorFromPoint(POINT{0,0}, MONITOR_DEFAULTTOPRIMARY));

    ::RegisterClassExW(&wc);
    hwnd = CreateWindowW(
        wc.lpszClassName, L"FinexAuth", WS_POPUP,
        (GetSystemMetrics(SM_CXSCREEN) / 2) - ((int)c::bg::size.x / 2),
        (GetSystemMetrics(SM_CYSCREEN) / 2) - ((int)c::bg::size.y / 2),
        (int)c::bg::size.x, (int)c::bg::size.y,
        nullptr, nullptr, wc.hInstance, nullptr);

    SetWindowLongA(hwnd, GWL_EXSTYLE, GetWindowLongA(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    MARGINS margins = {-1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    POINT mouse;
    rc = {0};
    GetWindowRect(hwnd, &rc);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImFontConfig cfg;
    cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint  |
                           ImGuiFreeTypeBuilderFlags_LightHinting    |
                           ImGuiFreeTypeBuilderFlags_LoadColor        |
                           ImGuiFreeTypeBuilderFlags_Bitmap;

    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 1.f);
    bool   done        = false;

    while (!done)
    {
        // ── Message pump ──────────────────────────────────────────────────────
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT) done = true;
        }
        if (done) break;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // ── New ImGui frame ───────────────────────────────────────────────────
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // ── Global style tweaks ───────────────────────────────────────────────
        ImGuiStyle* style = &ImGui::GetStyle();

        static float color[4] = { 0.f, 247.f / 255.f, 1.f, 1.f };
        c::accent = ImVec4(color[0], color[1], color[2], 1.f);


        style->WindowPadding = ImVec2(0, 0);
        style->ItemSpacing = ImVec2(20, 20);
        style->WindowBorderSize = 0;
        style->ScrollbarSize = 6.f;

        // Push global bright-text + separator colors
        ImGui::PushStyleColor(ImGuiCol_Text,      ImColor(220, 228, 240, 255).Value);
        ImGui::PushStyleColor(ImGuiCol_Separator, ImColor(50,  58,  72,  255).Value);

        // ── Root window ───────────────────────────────────────────────────────
        ImGui::SetNextWindowSize(ImVec2((float)c::bg::size.x, (float)c::bg::size.y));
        ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
        ImGui::Begin("##root", nullptr, window_flags);
        {
            const ImVec2 wpos   = ImGui::GetWindowPos();
            const ImVec2 wreg   = ImGui::GetContentRegionMax();
            ImDrawList*  dl     = ImGui::GetWindowDrawList();

            // Background
            dl->AddRectFilled(wpos, wpos + ImVec2(wreg), ImColor(12, 18, 24, 255), c::bg::rounding);

            // Top gradient accent bar
            dl->AddRectFilledMultiColor(
                wpos, wpos + ImVec2(wreg.x, 3.f),
                ImColor(0,200,210,255), ImColor(0,120,200,255),
                ImColor(0,120,200,255), ImColor(0,200,210,255));

           
            dl->AddText(wpos + ImVec2(22.f, 16.f), ImColor(0,200,210,255), "FINEX AUTHENTICATION");

            const char* dev_tag = "</> Dev : Finex";
            float tag_w = ImGui::CalcTextSize(dev_tag).x;

            float padding_right = 80.f;
            float target_x = ImGui::GetWindowWidth() - tag_w - padding_right;
            float target_y = 16.f; 
            dl->AddText(wpos + ImVec2(target_x, target_y), ImColor(60, 80, 100, 180), dev_tag);

            // ── Close [x] button ─────────────────────────────────────────────
            ImGui::SetCursorPos(ImVec2(wreg.x - 32.f, 8.f));
            ImGui::PushStyleColor(ImGuiCol_Button,        ImColor(0,0,0,0).Value);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(200,50,50,200).Value);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImColor(220,60,60,255).Value);
            ImGui::PushStyleColor(ImGuiCol_Text,          ImColor(150,150,160,255).Value);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
            if (ImGui::Button("x##close", ImVec2(24.f, 24.f))) done = true;
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(4);

            const float HDR_H      = 45.f;
            const float PANEL_H    = (float)c::bg::size.y - HDR_H;
            const float SIDEBAR_W  = 140.f;
            const float CONTENT_W  = (float)c::bg::size.x - SIDEBAR_W - 1.f; 
            const float INPUT_W    = CONTENT_W - 162.f;  

            if (Login_mneu)
            {
                
                ImGui::SetCursorPos(ImVec2(0.f, HDR_H));

                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(14, 18, 24, 255).Value);
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2(0.f, 0.f));

                if (ImGui::BeginChild("##auth_panel", ImVec2((float)c::bg::size.x, PANEL_H), false))
                {
                    ImVec2 pp = ImGui::GetWindowPos();

                   
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(13, 17, 23, 255).Value);
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.f);

                    if (ImGui::BeginChild("##sidebar", ImVec2(SIDEBAR_W, PANEL_H), false))
                    {
                        ImVec2 sp = ImGui::GetWindowPos();

                        
                        ImGui::GetWindowDrawList()->AddRectFilled(sp, sp + ImVec2(SIDEBAR_W, 70.f), ImColor(10, 14, 20, 255));
                      
                        ImGui::GetWindowDrawList()->AddRectFilled(sp, sp + ImVec2(3.f, 70.f), ImColor(0,200,210,255));
                        ImGui::GetWindowDrawList()->AddText(  sp + ImVec2(20.f, 28.f), ImColor(0,200,210,255), "FINEX");

                        ImGui::Dummy(ImVec2(0.f, 82.f)); 

                        const char* labels[] = { "   Login", "   Register", "   License" };

                        for (int i = 0; i < 3; ++i)
                        {
                            ImGui::PushID(i);
                            bool sel = (g_active_tab == i);

                            if (sel)
                            {
                                ImVec2 tl = ImGui::GetWindowPos() + ImGui::GetCursorPos();
                                ImGui::GetWindowDrawList()->AddRectFilled(
                                    tl, tl + ImVec2(4.f, 50.f), ImColor(0,200,210,255));
                                ImGui::PushStyleColor(ImGuiCol_Button,        ImColor(22,28,38,255).Value);
                                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(28,36,48,255).Value);
                                ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImColor(22,28,38,255).Value);
                                ImGui::PushStyleColor(ImGuiCol_Text,          ImColor(0,210,220,255).Value);
                            }
                            else
                            {
                                ImGui::PushStyleColor(ImGuiCol_Button,        ImColor(0,0,0,0).Value);
                                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(24,30,42,200).Value);
                                ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImColor(20,26,36,255).Value);
                                ImGui::PushStyleColor(ImGuiCol_Text,          ImColor(80,100,135,255).Value);
                            }

                            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
                            ImGui::SetCursorPosX(0.f);
                            if (ImGui::Button(labels[i], ImVec2(SIDEBAR_W, 50.f)))
                                g_active_tab = i;
                            ImGui::PopStyleVar();
                            ImGui::PopStyleColor(4);
                            ImGui::PopID();

                           
                            ImVec2 rl = ImGui::GetWindowPos() + ImGui::GetCursorPos();
                            ImGui::GetWindowDrawList()->AddLine(
                                rl, rl + ImVec2(SIDEBAR_W, 0.f), ImColor(25,32,44,255));
                            ImGui::Dummy(ImVec2(0.f, 1.f));
                        }
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleVar();
                    ImGui::PopStyleColor();

                    // Vertical divider
                    ImVec2 dv = pp + ImVec2(SIDEBAR_W, 0.f);
                    ImGui::GetWindowDrawList()->AddLine(
                        dv, dv + ImVec2(0.f, PANEL_H), ImColor(30,40,56,255));

                    // ── CONTENT AREA ──────────────────────────────────────────
                    ImGui::SameLine(0.f, 0.f);
                    ImGui::SetCursorPosX(SIDEBAR_W + 100.f);

                    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(0,0,0,0).Value);
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30.f, 28.f));
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2(8.f,  12.f));

                    if (ImGui::BeginChild("##content", ImVec2(CONTENT_W, PANEL_H), false))
                    {
                        // ── Section header helper ─────────────────────────────
                        auto DrawHeader = [&](const char* sub, const char* title)
                        {
                            ImGui::Dummy(ImVec2(0.f, 10.f));
                            ImGui::PushStyleColor(ImGuiCol_Text, ImColor(160,180,210,255).Value);
                            ImGui::Text("%s", sub);
                            ImGui::PopStyleColor();
                            ImGui::PushStyleColor(ImGuiCol_Text, ImColor(0,200,210,255).Value);
                            ImGui::Text("%s", title);
                            ImGui::PopStyleColor();
                            ImGui::Dummy(ImVec2(0.f, 4.f));
                            ImVec2 rl = ImGui::GetWindowPos() + ImGui::GetCursorPos();
                            ImGui::GetWindowDrawList()->AddLine(
                                rl, rl + ImVec2(INPUT_W, 0.f), ImColor(38,50,68,255));
                            ImGui::Dummy(ImVec2(0.f, 14.f));
                        };

                        // ── Labelled input helper ─────────────────────────────
                        auto LabeledInput = [&](const char* label, const char* id,
                                                char* buf, int bufsz,
                                                ImGuiInputTextFlags flags = 0)
                        {
                            ImGui::PushStyleColor(ImGuiCol_Text, ImColor(120,148,182,255).Value);
                            ImGui::Text("%s", label);
                            ImGui::PopStyleColor();
                            ImGui::SetNextItemWidth(INPUT_W);
                            PushInputStyle();
                            ImGui::InputText(id, buf, bufsz, flags);
                            PopInputStyle();
                        };
                        // Login Tab Using FinexAuth API
                        if (g_active_tab == 0)
                        {
                            DrawHeader("Welcome back", "LOGIN TO YOUR ACCOUNT");
                          
                           
                            float input_width = ImGui::GetWindowWidth() - (style->WindowPadding.x * 10.0f);

                            ImGui::InputTextEx("b", "Enter Your Username",  username, ARRAYSIZE( username), ImVec2(input_width, 40), NULL);
                          
                            ImGui::InputTextEx("c", "Enter Your Password",  password, ARRAYSIZE( password), ImVec2(input_width, 40), ImGuiInputTextFlags_Password);
                           
                                PushAccentButton();
                                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 45.f);
                                if (ImGui::Button("Authorization", ImVec2(250.f, 46.f)))
                                {
                                    std::thread([]()
                                        {

                                            FNXApp.login(username, password);

                                            if (FNXApp.response.success)
                                            {

                                                Login_mneu = false;
                                                Main_Menu = true;


                                            }
                                            else
                                            {

                                                MessageBoxA(NULL, FNXApp.response.message.c_str(), "Finex Auth - Error", MB_OK | MB_ICONERROR);
                                            }



                                        }).detach();
                                   
                                }
                                PopAccentButton();
                                
                           
                        }                       
                        // Register Tab Using FinexAuth API
                        else if (g_active_tab == 1)
                        {
                            DrawHeader("Get started free", "CREATE A NEW ACCOUNT");

                            float input_width = ImGui::GetWindowWidth() - (style->WindowPadding.x * 10.0f);
                            ImGui::InputTextEx("a", "Choose a Username",  username, ARRAYSIZE( username), ImVec2(input_width, 40), NULL);
                            
                            ImGui::InputTextEx("b", "Choose a Password",  password, ARRAYSIZE( password), ImVec2(input_width, 40), ImGuiInputTextFlags_Password);
                            
                            ImGui::InputTextEx("c", "License",  license_key, ARRAYSIZE(license_key), ImVec2(input_width, 40), NULL);
                           

                           
                                PushAccentButton();
                                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 45.f);
                                if (ImGui::Button("Create Account", ImVec2(250.f, 44.f)))
                                {
                                   std::thread([]() {

                                       FNXApp.regstr(username, password, license_key);
                                       if(FNXApp.response.success)
                                       {
                                           Login_mneu = false;
                                           Main_Menu = true;
                                       }
                                       else
                                       {
                                           MessageBoxA(NULL, FNXApp.response.message.c_str(), "Finex Auth - Error", MB_OK | MB_ICONERROR);
                                           
                                       }
                                       }).detach();
                                }
                                PopAccentButton();
                          

                          
                        }
                        //License Tab Using FinexAuth API
                        else if (g_active_tab == 2)
                        {
                            DrawHeader("Already have a key?", "ACTIVATE WITH LICENSE KEY");
                            ImGui::Dummy(ImVec2(0.f, 16.f));
                            float input_width = ImGui::GetWindowWidth() - (style->WindowPadding.x * 10.0f);

                            ImGui::InputTextEx("b", "Enter Your Username", license_key, ARRAYSIZE(license_key), ImVec2(input_width, 40), NULL);

                           
                            PushAccentButton();
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 45.f);
                            if (ImGui::Button("Authorization", ImVec2(250.f, 46.f)))
                            {
                                std::thread([]()
                                    {

                                        FNXApp.license(license_key);

                                        if (FNXApp.response.success)
                                        {

                                            Login_mneu = false;
                                            Main_Menu = true;


                                        }
                                        else
                                        {

                                            MessageBoxA(NULL, FNXApp.response.message.c_str(), "Finex Auth - Error", MB_OK | MB_ICONERROR);
                                        }



                                    }).detach();

                            }
                            PopAccentButton();
                        }

                        if (strlen(reg_error) > 0) {
                             ImGui::Dummy(ImVec2(0.f, 5.f));
                             ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 45.f);
                             ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 50, 50, 255).Value);
                             ImGui::TextWrapped("%s", reg_error);
                             ImGui::PopStyleColor();
                        }
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleVar(2);   
                    ImGui::PopStyleColor(); 
                }
                ImGui::EndChild(); 
                ImGui::PopStyleVar(3);   
                ImGui::PopStyleColor();  
            }
            if(Main_Menu)
            {
                
                const float CARD_W  = 560.f;
                const float CARD_H  = 310.f;
                const float CARD_X  = ((float)c::bg::size.x - CARD_W) / 2.f;
                const float CARD_Y  = HDR_H + (PANEL_H - CARD_H) / 2.f;
                const float MENU_IW = CARD_W - 80.f;   // input width inside card (40px pad each side)

                ImGui::SetCursorPos(ImVec2(CARD_X, CARD_Y));

                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImColor(20, 24, 30, 255).Value);
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(40.f, 28.f));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2(8.f,  12.f));

                if (ImGui::BeginChild("##menu_panel", ImVec2(CARD_W, CARD_H), false))
                {
                    ImVec2 mp = ImGui::GetWindowPos();

                    // Card border
                    ImGui::GetWindowDrawList()->AddRect(
                        mp, mp + ImVec2(CARD_W, CARD_H), ImColor(38,50,68,200), 10.f);

                    // Top cyan accent bar
                    ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
                        mp, mp + ImVec2(CARD_W, 3.f),
                        ImColor(0,200,210,255), ImColor(0,120,200,255),
                        ImColor(0,120,200,255), ImColor(0,200,210,255));

                    ImGui::Dummy(ImVec2(0.f, 8.f));

                    // ── Title row ──────────────────────────────────────────
                    ImGui::PushStyleColor(ImGuiCol_Text, ImColor(0,200,210,255).Value);
                    ImGui::Text("MAIN MENU");
                    ImGui::PopStyleColor();

                    ImVec2 rl = ImGui::GetWindowPos() + ImGui::GetCursorPos();
                    ImGui::GetWindowDrawList()->AddLine(
                        rl, rl + ImVec2(MENU_IW, 0.f), ImColor(38,50,68,255));
                    ImGui::Dummy(ImVec2(0.f, 14.f));

                    // ── Name row ───────────────────────────────────────────
                    ImGui::PushStyleColor(ImGuiCol_Text, ImColor(100,125,160,255).Value);
                    ImGui::Text("Logged in as:");
                    ImGui::PopStyleColor();
                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Text, ImColor(0,210,220,255).Value);
                    ImGui::Text("%s", g_logged_in_name.c_str());
                    ImGui::PopStyleColor();

                    ImGui::Dummy(ImVec2(0.f, 10.f));

                    // ── Text box ───────────────────────────────────────────
                    ImGui::PushStyleColor(ImGuiCol_Text, ImColor(110,138,175,255).Value);
                    ImGui::Text("Text Box:");
                    ImGui::PopStyleColor();
                    ImGui::SetNextItemWidth(MENU_IW);
                    PushInputStyle();
                    ImGui::InputTextEx("a", "Enter your license key", license_key, ARRAYSIZE(license_key), ImVec2(ImGui::GetContentRegionMax().x - style->WindowPadding.x, 40), NULL);
                    PopInputStyle();

                    ImGui::Dummy(ImVec2(0.f, 22.f));

                    // ── Logout button ──────────────────────────────────────
                    ImGui::PushStyleColor(ImGuiCol_Button,        ImColor(130,28,38,220).Value);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(170,38,50,255).Value);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImColor(195,48,62,255).Value);
                    ImGui::PushStyleColor(ImGuiCol_Text,          ImColor(255,215,215,255).Value);
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
                    if (ImGui::Button("Logout##logout_btn", ImVec2(MENU_IW, 42.f)))
                    {
                        Login_mneu = true;
                        Main_Menu = false;
                        
                    }
                    ImGui::PopStyleVar();
                    ImGui::PopStyleColor(4);
                }
                ImGui::EndChild();
                ImGui::PopStyleVar(3);
                ImGui::PopStyleColor();
            }
        }
        move_window();
        ImGui::End();

        // Pop the 2 global colors pushed before Begin()
        ImGui::PopStyleColor(2);

        // ── Render ────────────────────────────────────────────────────────────
        ImGui::Render();
        const float cc[4] = {
            clear_color.x * clear_color.w,
            clear_color.y * clear_color.w,
            clear_color.z * clear_color.w,
            clear_color.w
        };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, cc);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        HRESULT hr = g_pSwapChain->Present(1, 0);
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }

    // ── Cleanup ───────────────────────────────────────────────────────────────
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// D3D helpers
// ─────────────────────────────────────────────────────────────────────────────

bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount                        = 2;
    sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator   = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags                              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow                       = hWnd;
    sd.SampleDesc.Count                   = 1;
    sd.Windowed                           = TRUE;
    sd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    HRESULT res = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
        featureLevelArray, 2, D3D11_SDK_VERSION,
        &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags,
            featureLevelArray, 2, D3D11_SDK_VERSION,
            &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK) return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain)        { g_pSwapChain->Release();        g_pSwapChain        = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice)        { g_pd3dDevice->Release();        g_pd3dDevice        = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// ─────────────────────────────────────────────────────────────────────────────
// WndProc
// ─────────────────────────────────────────────────────────────────────────────

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED) return 0;
        g_ResizeWidth  = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
