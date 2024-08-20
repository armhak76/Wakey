///////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
    #include "MainWindow.g.cpp"
#endif

#include "CustomDialog.xaml.h"

///////////////////////////////////////////////////////////////////////////////

#define IDM_OPEN                    101
#define IDM_EXIT                    102

#define HOTKEY_ID                     1

///////////////////////////////////////////////////////////////////////////////
// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::wakey::implementation
{
    ///////////////////////////////////////////////////////////////////////////////

    WNDPROC     MainWindow::s_pfnWndProcNext = NULL;    
    UINT        MainWindow::s_uTOT           = ::RegisterWindowMessageW(L"TimeOut");
    UINT        MainWindow::s_uTBC           = 0;
    MainWindow* MainWindow::s_pThis          = nullptr;

    ///////////////////////////////////////////////////////////////////////////////

    MainWindow::MainWindow(_In_ bool bRestart)
        : m_bGoingBack(false)
    {
        // Xaml objects should not call InitializeComponent during construction.
        // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent

        s_pThis = this;

        m_dlgCaller =
            winrt::single_threaded_map<
                ::winrt::hstring,
                ::winrt::Microsoft::UI::Xaml::Controls::ContentDialog
            >();

        ::winrt::Microsoft::UI::Xaml::Window window =
            this->try_as<::winrt::Microsoft::UI::Xaml::Window>();

        m_keepAwake = winrt::make_self<KeepAwake>(window, s_uTOT);
        m_keepAwake->Start(!bRestart);

        m_powerSettings = winrt::make_self<PowerSettings>(window);
        m_powerSettings->GetOverlayScheme();
        m_powerSettings->SetOverlayScheme();
        m_powerSettings->Register();

        m_hotKey = winrt::make_self<HotKey>(window);
        m_hotKey->Register();

        InitMicrosoftStore(window);
        InitBluetoothDevices();
        
        ExtendsContentIntoTitleBar(TRUE);
        SetTitleBar(AppTitleBar());

        HWND hWnd = Wnd::GetHwnd(window);
        {
            Wnd::Center(hWnd);
            Wnd::RemoveStyle(
                hWnd, WS_MAXIMIZEBOX | WS_VISIBLE
            );

            s_pfnWndProcNext = (WNDPROC)SetWindowLongPtr(
                hWnd, GWLP_WNDPROC, (LONG_PTR)&WndProc
            );
            WINRT_ASSERT(s_pfnWndProcNext);

            CreateTrayWnd(hWnd, TRUE);
        }

        m_hSemaphore = CreateSemaphoreW(NULL, 1, 1, NULL);
        WINRT_ASSERT(m_hSemaphore);
    }

    ///////////////////////////////////////////////////////////////////////////////

    ::winrt::fire_and_forget MainWindow::InitMicrosoftStore(
        _In_::winrt::Microsoft::UI::Xaml::Window& window
    )
    {
        winrt::apartment_context ui_thread; // Capture calling context.
        co_await winrt::resume_background();

        m_msStore = winrt::make_self<MicrosoftStore>(window);

        co_await m_msStore->CheckSideLoading();
        co_await m_msStore->CheckPurchase();
        co_await ui_thread;

        ::winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem navviewItem =
            navigationView().SelectedItem().try_as<::winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();
        
        if ( navviewItem &&
            (navviewItem.Name() == L"General" ||
             navviewItem.Name() == L"Bluetooth"))
        {
            GoToPage(navviewItem.Name(), false, false);

            contentFrame().BackStack().RemoveAtEnd();
            navigationView().IsBackEnabled(
                contentFrame().BackStack().Size() > 0
            );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    ::winrt::fire_and_forget MainWindow::InitBluetoothDevices(VOID)
    {
        winrt::apartment_context ui_thread; // Capture calling context.
        co_await winrt::resume_background();

        m_blueTooth = winrt::make_self<BlueTooth>();
        WINRT_ASSERT(m_blueTooth);

        m_etBtDevChng = m_blueTooth->OnDeviceChanged(
            { get_weak(), &MainWindow::OnBluetoothDeviceChanged }
        );

        co_await ui_thread;
    }
    
    ///////////////////////////////////////////////////////////////////////////////

    winrt::fire_and_forget MainWindow::OnBluetoothDeviceChanged(
        _In_ winrt::hstring hstrId,
        _In_ BluetoothDeviceChange eBtDeviceChange
    )
    {
        winrt::apartment_context ui_thread; // Capture calling context.

        // We must update the collection on the UI thread 

        co_await ui_thread;

        if (eBtDeviceChange == BluetoothDeviceChange::DeviceUpdated)
        {
            ::winrt::hstring hstrName1 = Settings::Get(
                Settings::SettingType::BluetoothId, winrt::hstring(L"None")
            );

            if (hstrName1 != L"None")
            {
                ::winrt::hstring hstrName2 =
                    m_blueTooth->LookupDeviceNameById(hstrId);

                if (!hstrName2.empty() &&
                     hstrName2 == hstrName1)
                {
                    bool bWasConnected =
                        m_blueTooth->LookupWasDeviceConnectedById(hstrId);

                    bool bConnected =
                        m_blueTooth->LookupIsDeviceConnectedById(hstrId);

                    if (bWasConnected && !bConnected)
                        LockWorkStation();
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID MainWindow::Activated(
        _In_::winrt::Windows::Foundation::IInspectable const& window,
        _In_::winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(window);
        //UNREFERENCED_PARAMETER(args);

        if (args.WindowActivationState() == winrt::Microsoft::UI::Xaml::WindowActivationState::PointerActivated ||
            args.WindowActivationState() == winrt::Microsoft::UI::Xaml::WindowActivationState::CodeActivated)
        {
            static BOOL sbFirstShow = TRUE;
            if (sbFirstShow)
            {
                navigationView().IsPaneOpen(
                    (bool)Settings::Get(Settings::SettingType::PaneOpen, true)
                );
        
                winrt::hstring hstrTitle =
                    ::winrt::Windows::ApplicationModel::Package::Current().DisplayName();

                if (Program::IsRunAsAdmin())
                {
                    winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader resLoader;

                    Notifications::Show(
                        resLoader.GetString(L"resStatusUpdate"),
                        resLoader.GetString(L"resAppRestartedAsAdmin")
                    );

                    hstrTitle = hstrTitle + 
                        L" " + resLoader.GetString(L"resAppTitleAdmin");
                }           
                    
                tbAppTitle().Text(hstrTitle);

                /*
                SHSTOCKICONINFO shsii;
                shsii.cbSize = sizeof(SHSTOCKICONINFO);

                SHGetStockIconInfo(
                    SIID_SHIELD,
                    SHGSI_ICON | SHGSI_SMALLICON,
                    &shsii
                );

                ICONINFO iconinfo;
                GetIconInfo(shsii.hIcon, &iconinfo);
                HBITMAP hBitmap = iconinfo.hbmColor;

                DestroyIcon(shsii.hIcon);
                */
                sbFirstShow = FALSE;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID MainWindow::Closed(
        _In_::winrt::Windows::Foundation::IInspectable const& window,
        _In_::winrt::Microsoft::UI::Xaml::WindowEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(window);
        UNREFERENCED_PARAMETER(args);

        m_blueTooth->OnDeviceChanged(m_etBtDevChng);

        m_keepAwake     ->Close();
        m_blueTooth     ->Close();
        m_hotKey        ->Close();
        m_msStore       ->Close();
        m_powerSettings ->Close();

        Settings::Set(
            Settings::SettingType::PaneOpen,
            (bool)navigationView().IsPaneOpen()
        );

        TrayIcon::Destroy();

        WINRT_ASSERT(m_hSemaphore);
        CloseHandle(m_hSemaphore);

        ::winrt::Microsoft::UI::Xaml::Window wnd =
            window.try_as<::winrt::Microsoft::UI::Xaml::Window>();
        Wnd::SavePlacement(wnd);

        s_pThis = nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID MainWindow::NavViewSelChanged(
        _In_::winrt::Microsoft::UI::Xaml::Controls::NavigationView const& view,
        _In_::winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(view);

        if (!m_bGoingBack)
        {
            ::winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem navviewItem =
                args.SelectedItem().try_as<::winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>();

            if (navviewItem)
                GoToPage(navviewItem.Name(), false, false);
        }
        else
        {
            m_bGoingBack = false;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID MainWindow::NavViewBackRequested(
        _In_::winrt::Microsoft::UI::Xaml::Controls::NavigationView const& view,
        _In_::winrt::Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(view);
        UNREFERENCED_PARAMETER(args);
        
        if (contentFrame().CanGoBack())
        {
            m_bGoingBack = true;
            contentFrame().GoBack();

            winrt::Microsoft::UI::Xaml::Controls::Page page = 
                contentFrame().Content().try_as<winrt::Microsoft::UI::Xaml::Controls::Page>();
            if(page)
                GoToPage(winrt::get_class_name(page), true, true);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    template <typename To> auto MainWindow::GetCurrentPageAs(VOID)
    {
        return contentFrame().Content().try_as<To>();
    }

    ///////////////////////////////////////////////////////////////////////////////

    bool MainWindow::IsCurrentPageName(_In_ winrt::hstring hstrName)
    {
       return contentFrame().SourcePageType().Name == hstrName;
    }

    ///////////////////////////////////////////////////////////////////////////////

    LRESULT CALLBACK MainWindow::WndProc(
        _In_ HWND   hWnd,
        _In_ UINT   uMsg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam
    )
    {
        UNREFERENCED_PARAMETER(wParam);
        static BOOL sbFirstNotif = TRUE;

        //MainWindow* pWndMain = (MainWindow*)GetPropW(hWnd, s_pwszThis);
        //WINRT_ASSERT(pWndMain);

        if (uMsg == s_uTBC)
        {
            if (s_pThis)
                s_pThis->CreateTrayWnd(hWnd, FALSE);

            return FALSE;
        }
        else if (uMsg == s_uTOT)
        {
            if (s_pThis)
            {
                PageSettings pageSettings =
                    s_pThis->GetCurrentPageAs<PageSettings>();

                if (pageSettings)
                    pageSettings.TimeIntervalFinished();
            }

            return FALSE;
        }

        switch (uMsg)
        {
        case WM_DPICHANGED:
        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED:
        case WM_DISPLAYCHANGE:
            break;
        case WM_HOTKEY:
        case WM_LBUTTONDBLCLK:
            if (!::IsWindowVisible(hWnd))
                Wnd::Activate(hWnd);
            else
                Wnd::Hide(hWnd);
            break;
        case WM_POWERBROADCAST:
            if (!sbFirstNotif)
            {
                if (wParam == PBT_POWERSETTINGCHANGE)
                {
                    s_pThis->m_powerSettings->SetOverlayScheme();
                    if (s_pThis->m_keepAwake)
                        s_pThis->m_keepAwake->Reload();
                }
            }
            else
            {
                sbFirstNotif = FALSE;
            }
            return FALSE;
        case TRAYICON_MSG:
        {
            switch (lParam)
            {
            case WM_LBUTTONDBLCLK:
            {
                if (!::IsWindowVisible(hWnd))
                    Wnd::Activate(hWnd);
                else
                    Wnd::Hide(hWnd);
            }
            break;
            case WM_RBUTTONUP:
            {
                HMENU hMenu = CreatePopupMenu();
                _ASSERTE(hMenu);

                if (!::IsWindowVisible(hWnd))
                    InsertMenuW(hMenu, 0, MF_BYPOSITION | MF_STRING, IDM_OPEN, L"&Show");
                else
                    InsertMenuW(hMenu, 0, MF_BYPOSITION | MF_STRING, IDM_OPEN, L"&Hide");

                InsertMenuW(hMenu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
                InsertMenuW(hMenu, 2, MF_BYPOSITION | MF_STRING, IDM_EXIT, L"Ex&it");

                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hWnd);

                UINT uCmd = (UINT)TrackPopupMenu(
                    hMenu, TPM_RETURNCMD | TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL
                );

                DestroyMenu(hMenu);
                if (uCmd == IDM_OPEN)
                {
                    if (!::IsWindowVisible(hWnd))
                        Wnd::Activate(hWnd);
                    else
                        Wnd::Hide(hWnd);
                }
                else if (uCmd == IDM_EXIT)
                {
                    Program::Exit();
                }
                return TRUE;
            }
            break;
            };
        };
        break;
        case WM_SYSCOMMAND:
            if (wParam == SC_CLOSE)
            {
                if (Settings::Get(Settings::SideLoaded, false) ||
                    Settings::Get(Settings::SettingType::Purchased, false))
                {
                    WINRT_ASSERT(s_pThis->m_hSemaphore);
                    DWORD dwWait = WaitForMultipleObjectsEx(
                        1, &s_pThis->m_hSemaphore, FALSE, 0, TRUE
                    );

                    if (dwWait == WAIT_OBJECT_0)
                    {
                        ::ShowWindow(hWnd, SW_HIDE);
                        ReleaseSemaphore(
                            s_pThis->m_hSemaphore, 1, NULL
                        );
                    }
                }
                else
                {
                    s_pThis->PurchaseDialog();
                }

                return FALSE;
            }
            break;
        case WM_GETMINMAXINFO:
        {
            UINT dpi = ::GetDpiForWindow(hWnd);
            UINT uScalingFactor = dpi / USER_DEFAULT_SCREEN_DPI;

            PMINMAXINFO pMinMaxInfo = (PMINMAXINFO)lParam;

            pMinMaxInfo->ptMinTrackSize.x = 800 * uScalingFactor;
        //  pMinMaxInfo->ptMaxTrackSize.x = 800 * uScalingFactor;
            pMinMaxInfo->ptMinTrackSize.y = 600 * uScalingFactor;
        //  pMinMaxInfo->ptMaxTrackSize.y = 600 * uScalingFactor;

            return FALSE;
        }
        break;
        }

        return CallWindowProcW(
            s_pfnWndProcNext, hWnd, uMsg, wParam, lParam
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID MainWindow::CreateTrayWnd(_In_ HWND hWnd, _In_ BOOL bInit)
    {
        WINRT_ASSERT(hWnd);
        if (bInit)
        {
            s_uTBC = ::RegisterWindowMessageW(L"TaskbarButtonCreated");
            WINRT_ASSERT(s_uTBC);
            ChangeWindowMessageFilterEx(hWnd, s_uTBC, MSGFLT_ALLOW, NULL);
        }

        TrayIcon::Create(
            hWnd, Misc::LoadIcon(L"coffee_cup.ico")
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    MainWindow* MainWindow::Get(VOID)
    {
        return s_pThis;
        //return winrt::get_self<MainWindow>(
        //    winrt::Microsoft::UI::Xaml::Window::Current().try_as<wakey::MainWindow>()
        //);
    }

    ///////////////////////////////////////////////////////////////////////////////
    
    winrt::Windows::Foundation::IAsyncAction MainWindow::ShowDialog(
        _In_::winrt::Microsoft::UI::Xaml::Controls::ContentDialog const& contentDialog,
        _In_ winrt::hstring const& hstrFunction
    )
    {
        winrt::apartment_context ui_thread;
        co_await ui_thread;

        if (!m_dlgCaller.HasKey(hstrFunction))
            m_dlgCaller.Insert(hstrFunction, contentDialog);
        else
            co_return;
                
        co_await winrt::resume_on_signal(m_hSemaphore);
        {
            co_await ui_thread;

            ::winrt::Microsoft::UI::Xaml::Controls::ContentDialog dlg = 
                m_dlgCaller.TryLookup(hstrFunction);

            if(dlg)
                co_await dlg.ShowAsync();
        }
        m_dlgCaller.TryRemove(hstrFunction);
        ReleaseSemaphore(m_hSemaphore, 1, NULL);        
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::fire_and_forget MainWindow::PurchaseDialog(VOID)
    {
        PageAbout pageAbout = GetCurrentPageAs<PageAbout>();

        bool bIsOnAboutPage =
            pageAbout != nullptr;

        winrt::hstring hstrMessage;
        if (!bIsOnAboutPage)
            hstrMessage = L"resPurchaseMsg1";
        else
            hstrMessage = L"resPurchaseMsg2";

        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader resLoader;

        ::winrt::Microsoft::UI::Xaml::Controls::ContentDialog customDialog;
        customDialog = winrt::make<
            winrt::wakey::implementation::CustomDialog>(
                winrt::wakey::IconType::Info, resLoader.GetString(hstrMessage)
            );

        customDialog.XamlRoot(Content().XamlRoot());
        customDialog.DefaultButton(
            winrt::Microsoft::UI::Xaml::Controls::ContentDialogButton::Primary
        );

        if (!bIsOnAboutPage)
        {
            customDialog.PrimaryButtonText(L"Yes");
            customDialog.CloseButtonText(L"No");
        }
        else
            customDialog.CloseButtonText(L"OK");

        customDialog.PrimaryButtonClick([&](auto&& ...)
            {
                if(!bIsOnAboutPage)
                    GoToPage(L"About", false, true);
            });

        co_await ShowDialog(customDialog, __FUNCTIONW__);
        //co_await customDialog.ShowAsync();        
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID MainWindow::GoToPage(
        _In_ winrt::hstring hstrName,
        _In_ bool           bXamlTypeName,
        _In_ bool           bSelectOnly
    )
    {
        WINRT_ASSERT(!hstrName.empty());

        typedef struct __PAGENAVINFO
        {
            Windows::UI::Xaml::Interop::TypeName    typeName;
            uint32_t                                uIndex;
            LPCWSTR                                 pwszName;
            BOOL                                    bFooter;

        } PAGENAVINFO;

        PAGENAVINFO pni[] = {
            { xaml_typename<PageGeneral  >(), 0, L"General",   FALSE },
            { xaml_typename<PageSettings >(), 1, L"Settings",  FALSE },
            { xaml_typename<PageBluetooth>(), 2, L"Bluetooth", FALSE },
            { xaml_typename<PageAbout    >(), 0, L"About",     TRUE  },
        };

        PAGENAVINFO* pPNI;
        winrt::Windows::Foundation::IInspectable ii = nullptr;
        if (!hstrName.empty())
        {
            for (UINT i = 0; i < __crt_countof(pni); i++)
            {
                pPNI = &pni[i];
                if (bXamlTypeName)
                {
                    if (hstrName != pPNI->typeName.Name)
                        continue;
                }
                else
                {
                    if (hstrName != pPNI->pwszName)
                        continue;
                }

                navigationView().Header(
                    ::winrt::box_value(pPNI->pwszName)
                );

                if (bSelectOnly)
                {
                    if (!pPNI->bFooter)
                        ii = navigationView().MenuItems().GetAt(pPNI->uIndex);
                    else
                        ii = navigationView().FooterMenuItems().GetAt(pPNI->uIndex);

                    navigationView().SelectedItem(ii);
                }
                else
                {
                    contentFrame().Navigate(pPNI->typeName, nullptr);                    
                }

                navigationView().IsBackEnabled(
                    contentFrame().BackStack().Size() > 0
                );
                break;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
}

