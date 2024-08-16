///////////////////////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <unknwn.h>
#include <restrictederrorinfo.h>
#include <hstring.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <sstream>
#include "strsafe.h"


#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

///////////////////////////////////////////////////////////////////////////////

// Undefine GetCurrentTime macro to prevent
// conflict with Storyboard::GetCurrentTime
#undef GetCurrentTime
///////////////////////////////////////////////////////////////////////////////

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>

#include <winrt/Windows.ApplicationModel.Activation.h>

#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.Services.Store.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/windows.devices.radios.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Navigation.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/microsoft.ui.interop.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <winrt/Microsoft.Windows.AppLifecycle.h>

#include <microsoft.ui.dispatching.interop.h>
#include <microsoft.ui.xaml.window.h>

#include <winrt/Windows.System.h>

#include <Windows.h>
#pragma comment(lib, "User32.lib")

//#include <wil/cppwinrt_helpers.h>
//#include <wil/resource.h>

#include <winrt/Mntone.AngelUmbrella.Controls.h>

#include "MainWindow.xaml.h"

///////////////////////////////////////////////////////////////////////////////

#define TRAYICON_ID              1  // ID number for the Notify Icon
#define TRAYICON_MSG    WM_APP + 1  // The message ID sent to our window

#define KEEPAWAKE_OFF       0
#define KEEPAWAKE_ALWAYS    1
#define KEEPAWAKE_ON_PWR    2
#define KEEPAWAKE_ON_TIME   3


namespace winrt::wakey::implementation
{
///////////////////////////////////////////////////////////////////////////////

    struct Settings
    {
        enum SettingType
        {
            HotKey,
            HotKeyMod,
            Autorun,
            Bluetooth,
            BluetoothId,
            FirstRun,
            KeepScreenOn,
            Notifications,
            KeepAwake,
            Theme,
            Purchased,
            PaneOpen,
            TimeInterval,
            PowerOverlay,
            PowerOverlayScheme,
            BatteryPower,
            BatterySaver,
            DynamicLink,
            RunAsAdminAlways,
            SideLoaded,
            WindowPlacement,
        };

        static VOID Clear(VOID);

        static VOID Set(_In_ const winrt::param::hstring& key, _In_ const winrt::Windows::Foundation::IInspectable& value);
        static const winrt::Windows::Foundation::IInspectable Get(_In_ const winrt::param::hstring& key, _In_ const winrt::Windows::Foundation::IInspectable& default);
        static VOID Set(_In_ Settings::SettingType key, _In_ const winrt::Windows::Foundation::IInspectable& value);
        static const winrt::Windows::Foundation::IInspectable Get(_In_ Settings::SettingType key, _In_ const winrt::Windows::Foundation::IInspectable& default);

        static winrt::param::hstring SettingTypeToString(_In_ winrt::wakey::implementation::Settings::SettingType key);
        
        static VOID    Set(_In_ Settings::SettingType key, _In_ bool    value);
        static VOID    Set(_In_ Settings::SettingType key, _In_ int32_t value);
        static VOID    Set(_In_ Settings::SettingType key, _In_ const ::winrt::hstring& value);
        static bool    Get(_In_ Settings::SettingType key, _In_ bool    default);
        static int32_t Get(_In_ Settings::SettingType key, _In_ int32_t default);
        static const ::winrt::hstring Get(_In_ winrt::wakey::implementation::Settings::SettingType key, _In_ const ::winrt::hstring& default);

        static VOID     SetWindowPlacement(_In_ WINDOWPLACEMENT const& wp);
        static bool     GetWindowPlacement(_In_ WINDOWPLACEMENT& wp);
    };

///////////////////////////////////////////////////////////////////////////////

    struct Notifications
    {
        static BOOL     ShouldShow          (VOID);
        static VOID     Show                (_In_ ::winrt::param::hstring const& title, _In_ ::winrt::param::hstring const& text);
    };

///////////////////////////////////////////////////////////////////////////////

    struct Wnd
    {
        static BOOL         s_bFirst;

        static VOID         Center              (_In_ HWND hWnd);
        static WNDPROC      Subclass            (_In_ HWND hWnd, _In_ WNDPROC pfnWndProcNew);
        static VOID         RemoveStyle         (_In_ HWND hWnd, _In_ LONG_PTR dwExStyle);
        static VOID         Show                (_In_ HWND hWnd, _In_ bool bShow);
        static VOID         Activate            (_In_ HWND hWnd);
        static VOID         Activate            (_In_::winrt::Microsoft::UI::Xaml::Window const& window);
        static HWND         GetHwnd             (_In_::winrt::Microsoft::UI::Xaml::Window const& window);
        static VOID         SavePlacement       (_In_::winrt::Microsoft::UI::Xaml::Window const& window);
        static VOID         RestorePlacement    (_In_::winrt::Microsoft::UI::Xaml::Window const& window);
    };

///////////////////////////////////////////////////////////////////////////////

    struct Misc
    {
        static HICON    LoadIcon            (_In_ LPCWSTR lpwszFileName);
        static HICON    LoadPngAsIcon       (_In_ LPCWSTR lpwszFileName);
        static BOOL     IsDynamicLockEnabled(VOID);
        static BOOL     SetDynamicLockEnabled(_In_ BOOL bEnabled);
    };

///////////////////////////////////////////////////////////////////////////////

    struct Theme
    {
        static winrt::Microsoft::UI::Xaml::ApplicationTheme GetSystemDefault(VOID);
        static VOID AllowDarkModeForApp (VOID);
    };

    ///////////////////////////////////////////////////////////////////////////////

    struct Registry
    {
        static DWORD GetValueHKCU_DWORD(_In_ LPCWSTR pwszKey, _In_ LPCWSTR pwszValue, _In_ DWORD dwDefault);
        static BOOL  SetValueHKCU_DWORD(_In_ LPCWSTR pwszKey, _In_ LPCWSTR pwszValue, _In_ DWORD dwValue);
    };

///////////////////////////////////////////////////////////////////////////////

    struct TrayIcon
    {
        static NOTIFYICONDATA g_NID;

        static VOID     Create(_In_ HWND hWnd, _In_ HICON hIcon);
        static VOID     Destroy(VOID);
    };

///////////////////////////////////////////////////////////////////////////////

    struct Program
    {
        static VOID     Exit                (VOID);
        static VOID     Restart             (_In_::winrt::Microsoft::UI::Xaml::Window const& window, _In_ ::winrt::param::hstring const& args);
        static HANDLE   RunAsAdmin          (_In_opt_ LPCWSTR pwszArgs);
        static BOOL     IsRunAsAdmin        (VOID);
    };

///////////////////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////////

