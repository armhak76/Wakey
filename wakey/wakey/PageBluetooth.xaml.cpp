#include "pch.h"
#include "PageBluetooth.xaml.h"
#if __has_include("PageBluetooth.g.cpp")
    #include "PageBluetooth.g.cpp"
#endif

#include "App.xaml.h"
#include "MainWindow.xaml.h"

using namespace ::winrt;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

#define GLYPH_PHONE_OFF         L"\uEE64"
#define GLYPH_PHONE_ON          L"\uEE65"
#define GLYPH_LINK_ON           L"\uE72E"
#define GLYPH_LINK_OFF          L"\uE785"
#define GLYPH_RUNASADMIN_ON     L"\uEA18"
#define GLYPH_RUNASADMIN_OFF    L"\uE83D"

namespace winrt::wakey::implementation
{
    ///////////////////////////////////////////////////////////////////////////////

    winrt::hstring PageBluetooth::s_hstrDevIdNone = L"None";

    ///////////////////////////////////////////////////////////////////////////////

    PageBluetooth::PageBluetooth(VOID) 
        : m_bLoading(TRUE)
        , m_bBtWin(FALSE)
    {
        // Xaml objects should not call InitializeComponent during construction.
        // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent

        NavigationCacheMode(
            winrt::Microsoft::UI::Xaml::Navigation::NavigationCacheMode::Enabled
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageBluetooth::Loaded(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (m_bLoading)
        {
            bool bBluetoothIsOn = Settings::Get(
                Settings::SettingType::Bluetooth, false
            );

            if (!MainWindow::Get()->m_blueTooth->IsEnabled())
            {
                bBluetoothIsOn = false;
                tglBluetooth().IsEnabled(false);
                infoBar().IsOpen(true);
            }
            else
            {
                tglBluetooth().IsEnabled(true);
                infoBar().IsOpen(false);
            }

            fntIcoDynamicLink().Glyph(GLYPH_PHONE_OFF);
            if (bBluetoothIsOn)
            {
                cboLink          ().IsEnabled(true);
                stcDynamicLink   ().IsEnabled(true);
                stcNewDevice     ().IsEnabled(true);
                stcDynamicLockWin().IsEnabled(true);
                
                tglBluetooth().IsOn(true);
                
                prgBluetooth().IsActive(
                    MainWindow::Get()->m_blueTooth->IsEnabled()
                );
                OnBluetoothDeviceChanged(
                    winrt::hstring(), BluetoothDeviceChange::None
                );

                fntIcoLink().Glyph(GLYPH_LINK_ON);
            }
            else
            {
                cboLink          ().IsEnabled(false);
                stcDynamicLink   ().IsEnabled(false);
                stcNewDevice     ().IsEnabled(false);
                stcDynamicLockWin().IsEnabled(false);

                fntIcoLink().Glyph(GLYPH_LINK_OFF);
                fntIcoDynamicLink().Glyph(GLYPH_PHONE_OFF);

                prgBluetooth().IsActive(false);
                tglBluetooth().IsOn(false);
            }

            tglDynamicLock().IsOn(!Misc::IsDynamicLockEnabled());
            if (Settings::Get(Settings::SideLoaded, false))
            {   
                if (Program::IsRunAsAdmin())
                {
                    stcDynamicLock().IsEnabled(bBluetoothIsOn);
                    fntIcoDynamicLock().Glyph(GLYPH_RUNASADMIN_ON);
                }
                else
                {
                    stcDynamicLock().IsEnabled(false);
                    fntIcoDynamicLock().Glyph(GLYPH_RUNASADMIN_OFF);
                }
            }
            else
            {
                stcDynamicLock().IsEnabled(false);
                stcDynamicLock().Visibility(
                    winrt::Microsoft::UI::Xaml::Visibility::Collapsed
                );
            }

            m_etDevChng = MainWindow::Get()->m_blueTooth->OnDeviceChanged(
                { get_weak(), &PageBluetooth::OnBluetoothDeviceChanged }
            );
            m_etBtState = MainWindow::Get()->m_blueTooth->OnStatusChanged(
                { get_weak(), &PageBluetooth::OnBluetoothStatusChanged }
            );

            MonitorDynamicLock();
            m_bLoading = FALSE;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageBluetooth::OnNavigatedTo(
        _In_ winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(args);

        /*
        m_etDevChng = MainWindow::Get()->m_blueTooth->OnDeviceChanged(
            { get_weak(), &PageBluetooth::OnBluetoothDeviceChanged }
        );
        m_etBtState = MainWindow::Get()->m_blueTooth->OnStatusChanged(
            { get_weak(), &PageBluetooth::OnBluetoothStatusChanged }
        );
        */

        if (!m_bLoading)
        {
            if (Settings::Get(Settings::SideLoaded, false))
            {
                tglDynamicLock().IsOn(!Misc::IsDynamicLockEnabled());
                if (Program::IsRunAsAdmin())
                {
                    stcDynamicLock().IsEnabled(tglBluetooth().IsOn());
                    fntIcoDynamicLock().Glyph(GLYPH_RUNASADMIN_ON);
                }
                else
                {
                    stcDynamicLock().IsEnabled(false);
                    fntIcoDynamicLock().Glyph(GLYPH_RUNASADMIN_OFF);
                }
            }
            else
            {
                stcDynamicLock().IsEnabled(false);
                stcDynamicLock().Visibility(
                    winrt::Microsoft::UI::Xaml::Visibility::Collapsed
                );
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageBluetooth::OnNavigatedFrom(
        _In_ winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(args);
        /*
        MainWindow::Get()->m_blueTooth->OnDeviceChanged(
            m_etDevChng
        );
        MainWindow::Get()->m_blueTooth->OnStatusChanged(
            m_etBtState
        );
        */
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::fire_and_forget PageBluetooth::OnBluetoothStatusChanged(
        _In_ BluetoothStatus eBtStatus
    )
    {    
        if (!m_bLoading)
        {
            if (eBtStatus == BluetoothStatus::StatusEnabled)
            {
                DispatcherQueue().TryEnqueue([&](auto&& ...)
                    {
                        infoBar().IsOpen(false);                        
                        prgBluetoothWin().Visibility(
                            winrt::Microsoft::UI::Xaml::Visibility::Collapsed
                        );
                        tglBluetooth().IsEnabled(true);
                        //prgBluetooth().IsActive(true);
                    });
            }
            else if (eBtStatus == BluetoothStatus::StatusDisabled)
            {
                DispatcherQueue().TryEnqueue([&](auto&& ...)
                    {
                        infoBar().IsOpen(true);
                        tglBluetooth().IsOn(false);
                        tglBluetooth().IsEnabled(false);
                        prgBluetooth().IsActive(false);
                    });
            }
        }

        co_return;
    }
    
    ///////////////////////////////////////////////////////////////////////////////

    winrt::fire_and_forget PageBluetooth::MonitorDynamicLock(VOID)
    {
        if (!Settings::Get(Settings::SideLoaded, false))
            co_return;

        winrt::apartment_context ui_thread;
        winrt::weak_ref<PageBluetooth> pThisWeak = get_weak();

        co_await winrt::resume_background();

        HKEY   hKey   = NULL;
        HANDLE hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

        RegOpenKeyEx(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon",
            0,
            KEY_NOTIFY,
            &hKey
        );

        do
        {
            co_await winrt::resume_background();
            RegNotifyChangeKeyValue(
                hKey,
                TRUE,
                REG_NOTIFY_CHANGE_LAST_SET,
                hEvent,
                TRUE
            );

            co_await winrt::resume_on_signal(hEvent);
            co_await ui_thread;

            if (pThisWeak.get())
            {
                pThisWeak.get()->tglDynamicLock().IsOn(
                    !Misc::IsDynamicLockEnabled()
                );
            }
        }
        while (TRUE);

        CloseHandle(hEvent);
        RegCloseKey(hKey);
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::fire_and_forget PageBluetooth::OnBluetoothDeviceChanged(
        _In_ winrt::hstring hstrId,
        _In_ BluetoothDeviceChange eBtDeviceChange
    )
    {
        if (eBtDeviceChange == BluetoothDeviceChange::EnumStarted)
        {
            DispatcherQueue().TryEnqueue([&](auto&& ...)
            {
                if (!m_bLoading)
                    prgBluetooth().IsActive(true);
            });
        }
        else if (eBtDeviceChange == BluetoothDeviceChange::EnumComplete)
        {
            DispatcherQueue().TryEnqueue([&](auto&& ...)
            {
                if (!m_bLoading)
                    prgBluetooth().IsActive(false);
            });
        }
        
        if (eBtDeviceChange == BluetoothDeviceChange::EnumStopped)
            co_return;

        //winrt::apartment_context ui_thread; // Capture calling context.
        // We must update the collection on the UI thread
        //co_await ui_thread;

        UNREFERENCED_PARAMETER(hstrId);
        
        DispatcherQueue().TryEnqueue([&](auto&& ...)
        {
            if (!m_bLoading)
            {
                cboLink().IsEnabled(false);

                winrt::hstring hstrName;

                cboLink().Items().Clear();
                cboLink().Items().Append(winrt::box_value(s_hstrDevIdNone));

                ::winrt::Windows::Foundation::Collections::IObservableMap<
                    winrt::hstring, Windows::Foundation::IInspectable
                > btDevices =
                    MainWindow::Get()->m_blueTooth->GetDevices();

                for (winrt::Windows::Foundation::Collections::IKeyValuePair<winrt::hstring, Windows::Foundation::IInspectable> it : btDevices)
                {
                    hstrName =
                        MainWindow::Get()->m_blueTooth->LookupDeviceNameById(it.Key());

                    if (!hstrName.empty())
                    {
                        //if (MainWindow::Get()->m_blueTooth->LookupIsDeviceConnectedById(it.Key()))
                            cboLink().Items().Append(winrt::box_value(hstrName));
                    }
                }

                hstrName = Settings::Get(
                    Settings::SettingType::BluetoothId, s_hstrDevIdNone
                );

                ::winrt::Windows::Devices::Enumeration::DeviceInformation di =
                    MainWindow::Get()->m_blueTooth->LookupDeviceByName(hstrName);

                if (di != nullptr)
                {
                    fntIcoDynamicLink().Glyph(GLYPH_PHONE_ON);
                    cboLink().SelectedItem(winrt::box_value(hstrName));
                }
                else
                {
                    fntIcoDynamicLink().Glyph(GLYPH_PHONE_OFF);
                    Settings::Set(
                        Settings::SettingType::BluetoothId, s_hstrDevIdNone
                    );
                    cboLink().SelectedItem(::winrt::box_value(s_hstrDevIdNone));
                }
                                
                cboLink().IsEnabled(cboLink().Items().Size() > 1);
            }
        });

        co_return;
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageBluetooth::cboLinkSelChanged(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            if (cboLink().SelectedItem())
            {
                winrt::hstring hstrName =
                    cboLink().SelectedItem().as<winrt::hstring>();

                ::winrt::Windows::Devices::Enumeration::DeviceInformation di =
                    MainWindow::Get()->m_blueTooth->LookupDeviceByName(hstrName);

                if (di != nullptr)
                {
                    fntIcoDynamicLink().Glyph(GLYPH_PHONE_ON);
                    Settings::Set(
                        Settings::SettingType::BluetoothId,
                        hstrName
                    );
                }
                else
                {
                    fntIcoDynamicLink().Glyph(GLYPH_PHONE_OFF);
                    Settings::Set(
                        Settings::SettingType::BluetoothId,
                        s_hstrDevIdNone
                    );

                    if (hstrName != s_hstrDevIdNone)
                    {
                        cboLink().Items().RemoveAt(cboLink().SelectedIndex());
                        cboLink().SelectedItem(winrt::box_value(s_hstrDevIdNone));
                    }
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageBluetooth::tglBluetoothOnToggled(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            //OnDeviceChangedHandler(winrt::hstring());

            bool bBluetoothIsOn = tglBluetooth().IsOn();
            
            stcDynamicLink().IsEnabled(bBluetoothIsOn);
            stcNewDevice  ().IsEnabled(bBluetoothIsOn);

            if (Settings::Get(Settings::SideLoaded, false))
            {
                if (Program::IsRunAsAdmin())
                {
                    stcDynamicLock().IsEnabled(bBluetoothIsOn);
                    fntIcoDynamicLock().Glyph(GLYPH_RUNASADMIN_ON);
                }
            }
            stcDynamicLockWin().IsEnabled(bBluetoothIsOn);
            cboLink().SelectedItem(
                winrt::box_value(s_hstrDevIdNone)
            );
            
            Settings::Set(
                Settings::SettingType::Bluetooth,
                bBluetoothIsOn
            );
            
            if (bBluetoothIsOn)
            {
                //prgBluetooth().IsActive(true);
                fntIcoLink().Glyph(GLYPH_LINK_ON);

                OnBluetoothDeviceChanged(
                    winrt::hstring(), BluetoothDeviceChange::None
                );
            }
            else
            {
                prgBluetooth().IsActive(false);
                fntIcoLink().Glyph(GLYPH_LINK_OFF);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageBluetooth::tglDynamicLockOnToggled(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading &&
            Program::IsRunAsAdmin() &&
            Settings::Get(Settings::SideLoaded, false))
        {
            Misc::SetDynamicLockEnabled(
                !tglDynamicLock().IsOn()
            );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageBluetooth::btnLinkBluetoothClick(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            ::winrt::Windows::System::Launcher::LaunchUriAsync(
                ::winrt::Windows::Foundation::Uri(L"ms-settings:bluetooth")
            );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageBluetooth::btnDynamicLockClick(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            ::winrt::Windows::System::Launcher::LaunchUriAsync(
                ::winrt::Windows::Foundation::Uri(L"ms-settings:signinoptions-dynamiclock")
            );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageBluetooth::btnBluetoothWinClick(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            if (!MainWindow::Get()->m_blueTooth->IsEnabled())
            {
                prgBluetoothWin().Visibility(
                    winrt::Microsoft::UI::Xaml::Visibility::Visible
                );
                MainWindow::Get()->m_blueTooth->IsEnabled(true);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageBluetooth::listViewItemClick(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::Controls::ItemClickEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        
        Mntone::AngelUmbrella::Controls::SettingsPanel sp =
            args.ClickedItem().try_as<Mntone::AngelUmbrella::Controls::SettingsPanel>();

        MainWindow::Get()->GoToPage(sp.Name(), false, true);
    }

    ///////////////////////////////////////////////////////////////////////////////
}
