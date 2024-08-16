///////////////////////////////////////////////////////////////////////////////
#include "pch.h"
///////////////////////////////////////////////////////////////////////////////

#include "MainWindow.xaml.h"
#include "CustomDialog.xaml.h"

#include <WinUser.h>
#include <shellapi.h>
#include <Shlobj.h>
#pragma comment (lib, "Shell32.lib")
#include <commctrl.h>
#pragma comment (lib, "Comctl32.lib")

//Shlobj_core.h)

//////////////////////////////////////////////////////////////////////////////

using namespace winrt::Windows::UI::Notifications;
using namespace winrt::Windows::Data::Xml::Dom;

///////////////////////////////////////////////////////////////////////////////

namespace winrt::wakey::implementation
{
    ///////////////////////////////////////////////////////////////////////////////

    VOID Settings::Clear(VOID)
    {
        winrt::Windows::Storage::ApplicationDataContainer localSettings =
            winrt::Windows::Storage::ApplicationData::Current().LocalSettings();

        if (localSettings)
            localSettings.Values().Clear();
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID Settings::Set(
        _In_ const winrt::param::hstring& key,
        _In_ const winrt::Windows::Foundation::IInspectable& value
    )
    {
        winrt::Windows::Storage::ApplicationDataContainer localSettings =
            winrt::Windows::Storage::ApplicationData::Current().LocalSettings();

        if(localSettings)
            localSettings.Values().Insert(key, value);
    }

    ///////////////////////////////////////////////////////////////////////////////

    const winrt::Windows::Foundation::IInspectable Settings::Get(
        _In_ const winrt::param::hstring& key,
        _In_ const winrt::Windows::Foundation::IInspectable& default
    )
    {
        winrt::Windows::Storage::ApplicationDataContainer localSettings =
            winrt::Windows::Storage::ApplicationData::Current().LocalSettings();

        if (!localSettings || !localSettings.Values().HasKey(key))
            return default;

        return localSettings.Values().Lookup(key);
    }

    ///////////////////////////////////////////////////////////////////////////////

    bool Settings::GetWindowPlacement(
        _In_ WINDOWPLACEMENT& wp
    )
    {
        ::winrt::Windows::Storage::ApplicationDataContainer localSettings =
            winrt::Windows::Storage::ApplicationData::Current().LocalSettings();

        ::winrt::param::hstring hstrKey = 
            SettingTypeToString(Settings::SettingType::WindowPlacement);

        if (!localSettings ||
            !localSettings.Values().HasKey(hstrKey))
        {
            return false;
        }

        ::winrt::Windows::Foundation::IInspectable ii = 
            localSettings.Values().Lookup(hstrKey);

        ::winrt::Windows::Storage::ApplicationDataCompositeValue compositeVal =
            ii.try_as<::winrt::Windows::Storage::ApplicationDataCompositeValue>();
        
        if (!compositeVal)
            return false;

        wp.length                   = winrt::unbox_value<uint32_t>(compositeVal.Lookup(L"wp.length"));
        wp.flags                    = winrt::unbox_value<uint32_t>(compositeVal.Lookup(L"wp.flags"));
        wp.showCmd                  = winrt::unbox_value<uint32_t>(compositeVal.Lookup(L"wp.showCmd"));
        wp.ptMinPosition.x          = winrt::unbox_value<uint32_t>(compositeVal.Lookup(L"wp.ptMinPosition.x"));
        wp.ptMinPosition.y          = winrt::unbox_value<uint32_t>(compositeVal.Lookup(L"wp.ptMinPosition.y"));
        wp.ptMaxPosition.x          = winrt::unbox_value<uint32_t>(compositeVal.Lookup(L"wp.ptMaxPosition.x"));
        wp.ptMaxPosition.y          = winrt::unbox_value<uint32_t>(compositeVal.Lookup(L"wp.ptMaxPosition.y"));
        wp.rcNormalPosition.left    = winrt::unbox_value<uint32_t>(compositeVal.Lookup(L"wp.rcNormalPosition.left"));
        wp.rcNormalPosition.top     = winrt::unbox_value<uint32_t>(compositeVal.Lookup(L"wp.rcNormalPosition.top"));
        wp.rcNormalPosition.right   = winrt::unbox_value<uint32_t>(compositeVal.Lookup(L"wp.rcNormalPosition.right"));
        wp.rcNormalPosition.bottom  = winrt::unbox_value<uint32_t>(compositeVal.Lookup(L"wp.rcNormalPosition.bottom"));

        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID Settings::SetWindowPlacement(
        _In_ WINDOWPLACEMENT const& wp
    )
    {
        ::winrt::Windows::Storage::ApplicationDataCompositeValue compositeVal =
            ::winrt::Windows::Storage::ApplicationDataCompositeValue();

        compositeVal.Insert(L"wp.length",                   winrt::box_value((uint32_t)wp.length));
        compositeVal.Insert(L"wp.flags",                    winrt::box_value((uint32_t)wp.flags ));
        compositeVal.Insert(L"wp.showCmd",                  winrt::box_value((uint32_t)wp.showCmd));
        compositeVal.Insert(L"wp.ptMinPosition.x",          winrt::box_value((uint32_t)wp.ptMinPosition.x));
        compositeVal.Insert(L"wp.ptMinPosition.y",          winrt::box_value((uint32_t)wp.ptMinPosition.y));
        compositeVal.Insert(L"wp.ptMaxPosition.x",          winrt::box_value((uint32_t)wp.ptMaxPosition.x));
        compositeVal.Insert(L"wp.ptMaxPosition.y",          winrt::box_value((uint32_t)wp.ptMaxPosition.y));
        compositeVal.Insert(L"wp.rcNormalPosition.left",    winrt::box_value((uint32_t)wp.rcNormalPosition.left));
        compositeVal.Insert(L"wp.rcNormalPosition.top",     winrt::box_value((uint32_t)wp.rcNormalPosition.top));
        compositeVal.Insert(L"wp.rcNormalPosition.right",   winrt::box_value((uint32_t)wp.rcNormalPosition.right));
        compositeVal.Insert(L"wp.rcNormalPosition.bottom",  winrt::box_value((uint32_t)wp.rcNormalPosition.bottom));

        Set(Settings::SettingType::WindowPlacement, compositeVal);
    }

    ///////////////////////////////////////////////////////////////////////////////
    
    VOID Settings::Set(
        _In_ winrt::wakey::implementation::Settings::SettingType key,
        _In_ const winrt::Windows::Foundation::IInspectable& value
    )
    {
        Set(SettingTypeToString(key), value);
    }

    ///////////////////////////////////////////////////////////////////////////////

    const winrt::Windows::Foundation::IInspectable Settings::Get(
        _In_ winrt::wakey::implementation::Settings::SettingType key,
        _In_ const winrt::Windows::Foundation::IInspectable& default
    )
    {
        return Get(SettingTypeToString(key), default);
    }
    
    ///////////////////////////////////////////////////////////////////////////////

    ::winrt::param::hstring Settings::SettingTypeToString(
        _In_ winrt::wakey::implementation::Settings::SettingType key
    )
    {
        LPCWSTR aSettings[] =
        {
            L"HotKey",
            L"HotKeyMod",
            L"Autorun",
            L"Bluetooth",
            L"BluetoothId",
            L"FirstRun",
            L"KeepScreenOn",
            L"Notifications",
            L"KeepAwake",
            L"Theme",
            L"Purcahsed",
            L"PaneOpen",
            L"TimeInterval",
            L"PowerOverlay",
            L"PowerOverlayScheme",
            L"BatteryPower",
            L"BatterySaver",
            L"DynamicLink",
            L"RunAsAdminAlways",
            L"SideLoaded",
            L"WindowPlacement",
        };

        return aSettings[key];
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID Settings::Set(
        _In_ winrt::wakey::implementation::Settings::SettingType key,
        _In_ bool value
    )
    {
        Set(key, winrt::box_value<bool>(value));
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID Settings::Set(
        _In_ winrt::wakey::implementation::Settings::SettingType key,
        _In_ int32_t value
    )
    {
        Set(key, winrt::box_value<int32_t>(value));
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID Settings::Set(
        _In_ winrt::wakey::implementation::Settings::SettingType key,
        _In_ const ::winrt::hstring& value
    )
    {
        Set(key, winrt::box_value(value));
    }

    ///////////////////////////////////////////////////////////////////////////////

    bool Settings::Get(
        _In_ winrt::wakey::implementation::Settings::SettingType key,
        _In_ bool default)
    {
        return winrt::unbox_value<bool>(
            Get(key, winrt::box_value<bool>(default))
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    int32_t Settings::Get(
        _In_ winrt::wakey::implementation::Settings::SettingType key,
        _In_ int32_t default
    )
    {
        return winrt::unbox_value<int32_t>(
            Get(key, winrt::box_value<int32_t>(default))
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    const ::winrt::hstring Settings::Get(
        _In_ winrt::wakey::implementation::Settings::SettingType key,
        _In_ const ::winrt::hstring& default
    )
    {
        return winrt::unbox_value<::winrt::hstring>(
            Get(key, winrt::box_value(default))
        );
    }

///////////////////////////////////////////////////////////////////////////////

    BOOL Notifications::ShouldShow(VOID)
    {
        return Settings::Get(
            winrt::wakey::implementation::Settings::SettingType::Notifications, false
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID Notifications::Show(
        _In_ ::winrt::param::hstring const& title,
        _In_ ::winrt::param::hstring const& text
    )
    {
        if (!ShouldShow())
            return;

        winrt::Windows::Data::Xml::Dom::XmlDocument doc;
        doc.LoadXml(L"\
            <toast>\
                <visual>\
                    <binding template=\"ToastGeneric\">\
                        <text></text>\
                        <text></text>\
                    </binding>\
                </visual>\
            </toast>"
        );

        // Populate with text and values
        doc.DocumentElement().SetAttribute(L"launch", L"action=viewConversation&conversationId=9813");
        doc.SelectSingleNode(L"//text[1]").InnerText(title);
        doc.SelectSingleNode(L"//text[2]").InnerText(text);

        // Construct the notification
        winrt::Windows::UI::Notifications::ToastNotification notif{ doc };
        winrt::Windows::UI::Notifications::ToastNotificationManager toastManager{};
        winrt::Windows::UI::Notifications::ToastNotifier toastNotifier{ toastManager.CreateToastNotifier() };

        // And show it!
        toastNotifier.Show(notif);
    }

///////////////////////////////////////////////////////////////////////////////

    BOOL Wnd::s_bFirst = TRUE;

    ///////////////////////////////////////////////////////////////////////////////

    WNDPROC Wnd::Subclass(
        _In_ HWND hWnd,
        _In_ WNDPROC pfnWndProcNew
    )
    {
        WINRT_ASSERT(hWnd);
        WINRT_ASSERT(pfnWndProcNew);

        if (!hWnd || !pfnWndProcNew)
            return NULL;

        WNDPROC pfnWndProcOld = (WNDPROC)SetWindowLongPtr(
            hWnd, GWLP_WNDPROC, (LONG_PTR)&pfnWndProcNew
        );

        WINRT_ASSERT(pfnWndProcOld);
        return pfnWndProcOld;
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID Wnd::RemoveStyle(
        _In_ HWND hWnd,
        _In_ LONG_PTR dwExStyle
    )
    {
        WINRT_ASSERT(hWnd);
        WINRT_ASSERT(dwExStyle);

        if (!hWnd || !dwExStyle)
            return;

        SetWindowLongPtr(
            hWnd,
            GWL_STYLE,
            GetWindowLongPtr(hWnd, GWL_STYLE) & ~(dwExStyle)
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID Wnd::Center(
        _In_ HWND hWnd
    )
    {
        WINRT_ASSERT(hWnd);
        if (!hWnd)
            return;

        RECT rectClient;
        RECT rectWindow;

        ::GetClientRect(hWnd, &rectClient);
        ::GetWindowRect(hWnd, &rectWindow);

        INT posX =
            ::GetSystemMetrics(SM_CXSCREEN) / 2 -
            (rectWindow.right - rectWindow.left) / 2;

        INT posY =
            ::GetSystemMetrics(SM_CYSCREEN) / 2 -
            (rectWindow.bottom - rectWindow.top) / 2;

        ::MoveWindow(
            hWnd,
            posX,
            posY,
            rectClient.right  - rectClient.left,
            rectClient.bottom - rectClient.top,
            TRUE
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID Wnd::Show(
        _In_ HWND hWnd,
        _In_ bool bShow
    )
    {
        WINRT_ASSERT(hWnd);
        if (!hWnd)
            return;

        winrt::Microsoft::UI::WindowId winID =
            winrt::Microsoft::UI::GetWindowIdFromWindow(hWnd);

        winrt::Microsoft::UI::Windowing::AppWindow appWnd =
            winrt::Microsoft::UI::Windowing::AppWindow::GetFromWindowId(winID);

        appWnd.Show(bShow);
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID Wnd::Activate(
        _In_ winrt::Microsoft::UI::Xaml::Window const& window
    )
    {        
        RestorePlacement(window);
        window.Activate();
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID Wnd::SavePlacement(
        _In_::winrt::Microsoft::UI::Xaml::Window const& window
    )
    {
        if (window)
        {
            HWND hWnd = Wnd::GetHwnd(window);
            if (hWnd)
            {
                WINDOWPLACEMENT wp;
                if (::GetWindowPlacement(hWnd, &wp))
                    Settings::SetWindowPlacement(wp);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID Wnd::RestorePlacement(
        _In_::winrt::Microsoft::UI::Xaml::Window const& window
    )
    {
        if (s_bFirst)
        {
            if (window)
            {
                HWND hWnd = Wnd::GetHwnd(window);
                if (hWnd)
                {
                    WINDOWPLACEMENT wp;
                    if (Settings::GetWindowPlacement(wp))
                        ::SetWindowPlacement(hWnd, &wp);
                }
            }

            s_bFirst = FALSE;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID Wnd::Activate(
        _In_ HWND hWnd
    )
    {
        WINRT_ASSERT(::IsWindow(hWnd));
        if (!::IsWindow(hWnd))
            return;

        if(s_bFirst)
        {
            WINDOWPLACEMENT wp;
            if (Settings::GetWindowPlacement(wp))
                ::SetWindowPlacement(hWnd, &wp);

            s_bFirst = FALSE;
        }

        if (::IsIconic(hWnd))
            ShowWindow(hWnd, SW_RESTORE);
        else if (!IsWindowVisible(hWnd))
            Wnd::Show(hWnd, TRUE);

        SetForegroundWindow(hWnd);
    }

    ///////////////////////////////////////////////////////////////////////////////

    HWND Wnd::GetHwnd(
        _In_ ::winrt::Microsoft::UI::Xaml::Window const& window
    )
    {
        HWND hWnd = NULL;
        
        ::winrt::com_ptr<IWindowNative> ptrIWindowNative =
            window.try_as<IWindowNative>();

        if (ptrIWindowNative)
            check_hresult(ptrIWindowNative->get_WindowHandle(&hWnd));

        WINRT_ASSERT(hWnd);
        return hWnd;
    }

///////////////////////////////////////////////////////////////////////////////

    HICON Misc::LoadIcon(_In_ LPCWSTR lpwszFileName)
    {
        WINRT_ASSERT(lpwszFileName);
        if (!lpwszFileName)
            return NULL;

        hstring strPath =
            winrt::Windows::ApplicationModel::Package::Current().InstalledLocation().Path() +
            L"\\Images\\" +
            lpwszFileName;

        return (HICON)::LoadImageW(
            NULL,
            strPath.c_str(),
            IMAGE_ICON,
            0,
            0,
            LR_LOADFROMFILE | LR_SHARED | LR_DEFAULTSIZE
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    HICON Misc::LoadPngAsIcon(_In_ LPCWSTR lpwszFileName)
    {
        HICON hIcon = NULL;

        WINRT_ASSERT(lpwszFileName);
        if (!lpwszFileName)
            return hIcon;

        hstring strPath =
            winrt::Windows::ApplicationModel::Package::Current().InstalledLocation().Path() +
            L"\\Images\\" +
            lpwszFileName;

        HANDLE hFile = CreateFileW(
            strPath.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
        );

        WINRT_ASSERT(hFile != INVALID_HANDLE_VALUE);
        WINRT_ASSERT(hFile != NULL);

        if (hFile != INVALID_HANDLE_VALUE)
        {
            DWORD dwSize = GetFileSize(hFile, NULL);
            PBYTE pbFile = (PBYTE)HeapAlloc(
                GetProcessHeap(),
                HEAP_ZERO_MEMORY,
                sizeof(BYTE) * dwSize
            );

            WINRT_ASSERT(pbFile);
            if (pbFile)
            {
                DWORD dwRead;
                if (ReadFile(hFile, pbFile, dwSize, &dwRead, NULL))
                {
                    hIcon = CreateIconFromResourceEx(
                        pbFile,
                        dwSize,
                        TRUE,
                        0x00030000,
                        0,
                        0,
                        LR_DEFAULTCOLOR
                    );
                }
                HeapFree(
                    GetProcessHeap(), 0, pbFile
                );
            }
            CloseHandle(hFile);
        }

        WINRT_ASSERT(hIcon);
        return hIcon;
    }

    ///////////////////////////////////////////////////////////////////////////////

    BOOL Misc::IsDynamicLockEnabled(void)
    {
        return Registry::GetValueHKCU_DWORD(
            L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon",
            L"EnableGoodbye",
            0
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    BOOL Misc::SetDynamicLockEnabled(_In_ BOOL bEnabled)
    {
        return Registry::SetValueHKCU_DWORD(
            L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon",
            L"EnableGoodbye",
            bEnabled
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::Microsoft::UI::Xaml::ApplicationTheme Theme::GetSystemDefault(VOID)
    {
        DWORD dwValue = Registry::GetValueHKCU_DWORD(
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"AppsUseLightTheme",
            0
        );

        if(dwValue)
            return winrt::Microsoft::UI::Xaml::ApplicationTheme::Light;
        
        return winrt::Microsoft::UI::Xaml::ApplicationTheme::Dark;
    }

    ///////////////////////////////////////////////////////////////////////////////

    DWORD Registry::GetValueHKCU_DWORD(
        _In_ LPCWSTR pwszKey,
        _In_ LPCWSTR pwszValue,
        _In_ DWORD   dwDefault
    )
    {
        DWORD dwType = REG_DWORD;
        DWORD dwValue = 0;
        DWORD dwSize = sizeof(DWORD);

        WINRT_ASSERT(pwszKey);
        WINRT_ASSERT(pwszValue);

        LSTATUS lStatus = SHGetValueW(
            HKEY_CURRENT_USER,
            pwszKey,
            pwszValue,
            &dwType,
            &dwValue,
            &dwSize
        );

        if (lStatus != ERROR_SUCCESS ||
            dwType  != REG_DWORD     ||
            dwSize  != sizeof(DWORD))
        {
            dwValue = dwDefault;
        }

        return dwValue;
    }

    ///////////////////////////////////////////////////////////////////////////////

    BOOL Registry::SetValueHKCU_DWORD(
        _In_ LPCWSTR pwszKey,
        _In_ LPCWSTR pwszValue,
        _In_ DWORD   dwValue
    )
    {
        WINRT_ASSERT(pwszKey);
        WINRT_ASSERT(pwszValue);

        return !SHSetValueW(
            HKEY_CURRENT_USER,
            pwszKey,
            pwszValue,
            REG_DWORD,
            (LPCVOID)&dwValue,
            sizeof(dwValue)
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID Theme::AllowDarkModeForApp(VOID)
    {
        HMODULE hUxTheme = LoadLibraryExW(
            L"uxtheme.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32
        );

        if (hUxTheme)
        {
            typedef BOOL(WINAPI* PFNALLOWDARMODEFORAPP)(BOOL bAllow);

            PFNALLOWDARMODEFORAPP pfnAllowDarkModeForApp =
                (PFNALLOWDARMODEFORAPP)GetProcAddress(
                    hUxTheme, MAKEINTRESOURCEA(135)
                );

            if (pfnAllowDarkModeForApp)
            {
                __try
                {
                    pfnAllowDarkModeForApp(1);
                }
                __finally
                {
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    NOTIFYICONDATA TrayIcon::g_NID = { 0 };

    VOID TrayIcon::Create(
        _In_ HWND hWnd,
        _In_ HICON hIcon
    )
    {
        SecureZeroMemory(&g_NID, sizeof(g_NID));

        g_NID.uID               = TRAYICON_ID;
        g_NID.uFlags            = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        g_NID.hWnd              = hWnd;
        g_NID.uCallbackMessage  = TRAYICON_MSG;
        g_NID.hIcon             = hIcon;

        StringCchCopyW(
            g_NID.szTip,
            __crt_countof(g_NID.szTip),
            ::winrt::Windows::ApplicationModel::Package::Current().DisplayName().c_str()
        );

        Shell_NotifyIcon(NIM_ADD, &g_NID);

        if (g_NID.hIcon && DestroyIcon(g_NID.hIcon))
            g_NID.hIcon = NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID TrayIcon::Destroy(VOID)
    {
        if (g_NID.hIcon)
        {
            DestroyIcon(g_NID.hIcon);
            g_NID.hIcon = NULL;
        }

        g_NID.uFlags = 0;
        Shell_NotifyIcon(NIM_DELETE, &g_NID);
    }

    ///////////////////////////////////////////////////////////////////////////////
 
///////////////////////////////////////////////////////////////////////////////

    VOID Program::Exit(VOID)
    {
        ::winrt::Microsoft::UI::Xaml::Application::Current().Exit();
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID Program::Restart(
        _In_::winrt::Microsoft::UI::Xaml::Window const& window,
        _In_ ::winrt::param::hstring const& args)
    {
        Wnd::SavePlacement(window);
        winrt::Microsoft::Windows::AppLifecycle::AppInstance::Restart(args);
    }

    ///////////////////////////////////////////////////////////////////////////////

    HANDLE Program::RunAsAdmin(_In_opt_ LPCWSTR pwszArgs)
    {
        SHELLEXECUTEINFOW sheiw;
        SecureZeroMemory(&sheiw, sizeof(sheiw));

        WCHAR wszFullPath[MAX_PATH];
        WCHAR wszFileName[MAX_PATH];
        SecureZeroMemory(wszFullPath, sizeof(wszFullPath));
        SecureZeroMemory(wszFileName, sizeof(wszFileName));

        SHGetFolderPathW(
            NULL,
            CSIDL_LOCAL_APPDATA,
            NULL,
            SHGFP_TYPE_CURRENT,
            wszFullPath
        );

        PathAppendW(wszFullPath, L"Microsoft\\WindowsApps");        
        GetModuleFileNameW(
            NULL, wszFileName, __crt_countof(wszFileName)
        );
        PathStripPathW(wszFileName);        
        PathAppendW(wszFullPath, wszFileName);

        sheiw.cbSize        = sizeof(SHELLEXECUTEINFO);
        sheiw.fMask         = SEE_MASK_NOCLOSEPROCESS;
        sheiw.lpVerb        = L"runas";
        sheiw.lpFile        = wszFullPath;
        sheiw.lpParameters  = pwszArgs;
        sheiw.nShow         = SW_SHOWNORMAL;
                
        ShellExecuteExW(&sheiw);
        return sheiw.hProcess;
    }

    ///////////////////////////////////////////////////////////////////////////////

    BOOL Program::IsRunAsAdmin(VOID)
    {
        HANDLE hToken  = NULL;
        BOOL   fRetval = OpenProcessToken(
            GetCurrentProcess(), TOKEN_QUERY, &hToken
        );

        if(fRetval)
        {
            DWORD cbSize = sizeof(TOKEN_ELEVATION);

            TOKEN_ELEVATION tokElevation;
            SecureZeroMemory(&tokElevation, cbSize);
            
            fRetval = GetTokenInformation(
                hToken,
                TokenElevation,
                &tokElevation,
                sizeof(tokElevation),
                &cbSize
            );

            if (fRetval)
                fRetval = tokElevation.TokenIsElevated;

            CloseHandle(hToken);
        }
       
        return fRetval;
    }
    
    ///////////////////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////////


