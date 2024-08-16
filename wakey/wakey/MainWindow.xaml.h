#pragma once

#include "MainWindow.g.h"
#include "App.xaml.h"

#include "MicrosoftStore.h"
#include "Bluetooth.h"
#include "KeepAwake.h"
#include "HotKey.h"
#include "PowerSettings.h"

namespace winrt::wakey::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
    private:
        bool   m_bGoingBack;
        HANDLE m_hSemaphore;

        static UINT s_uTBC;  
        
    public:
        static UINT        s_uTOT;
        static MainWindow* s_pThis;

//      winrt::event_token m_etOffLicChanged;

        MainWindow(_In_ bool bRestart);
        
        VOID Activated              (_In_ ::winrt::Windows::Foundation::IInspectable const& window, _In_ ::winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const& args);
        VOID Closed                 (_In_ ::winrt::Windows::Foundation::IInspectable const& window, _In_ ::winrt::Microsoft::UI::Xaml::WindowEventArgs const& args);
        VOID NavViewSelChanged      (_In_ ::winrt::Microsoft::UI::Xaml::Controls::NavigationView const& view, _In_ ::winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);
        VOID NavViewBackRequested   (_In_ ::winrt::Microsoft::UI::Xaml::Controls::NavigationView const& view, _In_ ::winrt::Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const& args);

        VOID GoToPage               (_In_ winrt::hstring hstrName, _In_ bool bXamlTypeName, _In_ bool bSelectOnly);

        ::winrt::Windows::Foundation::Collections::IMap<winrt::hstring, ::winrt::Microsoft::UI::Xaml::Controls::ContentDialog> m_dlgCaller;
        ::winrt::Windows::Foundation::IAsyncAction ShowDialog(_In_::winrt::Microsoft::UI::Xaml::Controls::ContentDialog const& contentDialog, _In_ winrt::hstring const& hstrFunction);

        ::winrt::fire_and_forget InitMicrosoftStore     (_In_ ::winrt::Microsoft::UI::Xaml::Window& window);
        ::winrt::fire_and_forget InitBluetoothDevices   (VOID);
        ::winrt::fire_and_forget InitKeepAwake          (_In_::winrt::Microsoft::UI::Xaml::Window& window, _In_ BOOL bFirst); 
        ::winrt::fire_and_forget PurchaseDialog         (VOID);

        template <typename To> auto GetCurrentPageAs(VOID);
        bool IsCurrentPageName(_In_ winrt::hstring hstrName);
               
        winrt::com_ptr<MicrosoftStore>      m_msStore;
        winrt::com_ptr<BlueTooth>           m_blueTooth;
        winrt::com_ptr<KeepAwake>           m_keepAwake;
        winrt::com_ptr<HotKey>              m_hotKey;
        winrt::com_ptr<PowerSettings>       m_powerSettings;

        //winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader  m_resLoader;

        static MainWindow* Get(VOID);

    private:
        winrt::event_token m_etBtDevChng;
        winrt::fire_and_forget OnBluetoothDeviceChanged(_In_ winrt::hstring hstrId, _In_ BluetoothDeviceChange eBtDeviceChange);

        VOID CreateTrayWnd  (_In_ HWND hWnd, _In_ BOOL bInit);

        static WNDPROC s_pfnWndProcNext;
        static LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
    };
}

namespace winrt::wakey::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}

