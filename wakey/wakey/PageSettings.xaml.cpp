#include "pch.h"
#include "PageSettings.xaml.h"
#if __has_include("PageSettings.g.cpp")
    #include "PageSettings.g.cpp"
#endif

#include "App.xaml.h"

using namespace winrt;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

#define GLYPH_KEEPSCREEN_ON     L"\uE7F4"
#define GLYPH_KEEPSCREEN_OFF    L"\uEA14"
#define GLYPH_POWERMODE_ON      L"\uEC4A"
#define GLYPH_POWERMODE_OFF     L"\uEC48"
#define GLYPH_BATTERYPOWER_ON   L"\uEBA6"
#define GLYPH_BATTERYPOWER_OFF  L"\uEC02"
#define GLYPH_BATTERYSAVER_ON   L"\uEBBC"
#define GLYPH_BATTERYSAVER_OFF  L"\uEBAA"
#define GLYPH_KEEPAWAKE_ON      L"\uE7E8"
#define GLYPH_KEEPAWAKE_OFF     L"\uF83D"

namespace winrt::wakey::implementation
{
    ///////////////////////////////////////////////////////////////////////////////

    PageSettings::PageSettings(VOID) 
        : m_bLoading(TRUE)
    {
        // Xaml objects should not call InitializeComponent during construction.
        // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent     
        
        NavigationCacheMode(
            winrt::Microsoft::UI::Xaml::Navigation::NavigationCacheMode::Enabled
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageSettings::Loaded(
        _In_ ::winrt::Windows::Foundation::IInspectable const& sender,
        _In_ ::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);
        
        if (m_bLoading)
        {
            if (MainWindow::Get()->m_powerSettings)
            {
                if (!MainWindow::Get()->m_powerSettings->IsBatteryPresent())
                {
                    tglBatteryPower().IsOn(false);
                    stcBatteryPower().IsEnabled(false);
                    tglBatterySaver().IsOn(false);
                    stcBatterySaver().IsEnabled(false);

                    steBattery().Visibility(
                        winrt::Microsoft::UI::Xaml::Visibility::Collapsed
                    );
                    txtBattery().Visibility(
                        winrt::Microsoft::UI::Xaml::Visibility::Collapsed
                    );
                }
            }

            bool bKeepAwake = Settings::Get(
                Settings::SettingType::KeepAwake, false
            );

            tglEnableWakey  ().IsOn(bKeepAwake);

            stcKeepScreenOn ().IsEnabled(bKeepAwake);
            stcPowerMode    ().IsEnabled(bKeepAwake);
            stcTimeInterval ().IsEnabled(bKeepAwake);
            stcBatteryPower ().IsEnabled(bKeepAwake);
            stcPowerSettings().IsEnabled(bKeepAwake);
            
            cboTimeInterval ().SelectedIndex(0);
            Settings::Set(
                Settings::SettingType::TimeInterval, 0
            );

            if (bKeepAwake)
            {
                tglKeepScreenOn().IsOn(
                    Settings::Get(Settings::SettingType::KeepScreenOn, false)
                );
                tglBatteryPower().IsOn(
                    Settings::Get(Settings::SettingType::BatteryPower, false)
                );
                tglBatterySaver().IsOn(
                    Settings::Get(Settings::SettingType::BatterySaver, false)
                );
                tglPowerMode().IsOn(
                    Settings::Get(Settings::SettingType::PowerOverlay, false)
                );
                stcBatterySaver().IsEnabled(
                    tglBatteryPower().IsOn()
                );

                fntIcoKeepAwake().Glyph(GLYPH_KEEPAWAKE_ON);
            }
            else
            {
                fntIcoKeepAwake().Glyph(GLYPH_KEEPAWAKE_OFF);
            }

            if (tglKeepScreenOn().IsOn())
                fntIcoKeepScreenOn().Glyph(GLYPH_KEEPSCREEN_ON);
            else
                fntIcoKeepScreenOn().Glyph(GLYPH_KEEPSCREEN_OFF);

            if (tglPowerMode().IsOn())
                fntIcoPowerMode().Glyph(GLYPH_POWERMODE_ON);
            else
                fntIcoPowerMode().Glyph(GLYPH_POWERMODE_OFF);

            if (tglBatteryPower().IsOn())
                fntIcoBatteryPower().Glyph(GLYPH_BATTERYPOWER_ON);
            else
                fntIcoBatteryPower().Glyph(GLYPH_BATTERYPOWER_OFF);

            if (tglBatterySaver().IsOn())
                fntIcoBatterySaver().Glyph(GLYPH_BATTERYSAVER_ON);
            else
                fntIcoBatterySaver().Glyph(GLYPH_BATTERYSAVER_OFF);

            m_bLoading = FALSE;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageSettings::OnNavigatedTo(
        _In_ winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(args);
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageSettings::OnNavigatedFrom(
        _In_ winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(args);
    }

    ///////////////////////////////////////////////////////////////////////////////
    
    VOID PageSettings::tglEnableWakeyOnToggled(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            
            bool bEnabled = tglEnableWakey().IsOn();

            stcKeepScreenOn().IsEnabled(bEnabled);
            stcPowerMode   ().IsEnabled(bEnabled);
            stcTimeInterval().IsEnabled(bEnabled);
            stcBatteryPower().IsEnabled(bEnabled);
            //stcBatterySaver().IsEnabled(bEnabled);
            stcPowerSettings().IsEnabled(bEnabled);

            cboTimeInterval().SelectedIndex(0);
            Settings::Set(
                Settings::SettingType::TimeInterval, 0
            );
            
            if (bEnabled)
            {
                tglKeepScreenOn().IsOn(
                    Settings::Get(Settings::SettingType::KeepScreenOn, false)
                );                
                tglBatteryPower().IsOn(
                    Settings::Get(Settings::SettingType::BatteryPower, false)
                );
                tglBatterySaver().IsOn(
                    Settings::Get(Settings::SettingType::BatterySaver, false)
                );
                tglPowerMode().IsOn(
                    Settings::Get(Settings::SettingType::PowerOverlay, false)
                );
            }
            else
            {
                tglKeepScreenOn().IsOn(false);                
                tglBatteryPower().IsOn(false);
                tglBatterySaver().IsOn(false);
                tglPowerMode   ().IsOn(false);
            }

            if (bEnabled)
                fntIcoKeepAwake().Glyph(GLYPH_KEEPAWAKE_ON);
            else
                fntIcoKeepAwake().Glyph(GLYPH_KEEPAWAKE_OFF);

            Settings::Set(
                Settings::SettingType::KeepAwake, bEnabled
            );

            if (!m_bLoading && MainWindow::Get()->m_keepAwake)
                MainWindow::Get()->m_keepAwake->Reload();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageSettings::tglKeepScreenOnToggled(
        _In_ ::winrt::Windows::Foundation::IInspectable const& sender,
        _In_ ::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            if (tglKeepScreenOn().IsOn())
                fntIcoKeepScreenOn().Glyph(GLYPH_KEEPSCREEN_ON);
            else
                fntIcoKeepScreenOn().Glyph(GLYPH_KEEPSCREEN_OFF);

            Settings::Set(
                Settings::SettingType::KeepScreenOn,
                tglKeepScreenOn().IsOn()
            );

            if (MainWindow::Get()->m_keepAwake)
                MainWindow::Get()->m_keepAwake->Reload();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    
    VOID PageSettings::tglPowerModeOnToggled(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            if (tglPowerMode().IsOn())
                fntIcoPowerMode().Glyph(GLYPH_POWERMODE_ON);
            else
                fntIcoPowerMode().Glyph(GLYPH_POWERMODE_OFF);

            Settings::Set(
                Settings::SettingType::PowerOverlay,
                tglPowerMode().IsOn()
            );
            
            if(MainWindow::Get()->m_powerSettings)
                MainWindow::Get()->m_powerSettings->SetOverlayScheme();
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    
    VOID PageSettings::btnPowerSettingsClick(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            ::winrt::Windows::System::Launcher::LaunchUriAsync(
                ::winrt::Windows::Foundation::Uri(L"ms-settings:powersleep")
            );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageSettings::cboTimeIntervalSelChanged(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            Settings::Set(
                Settings::SettingType::TimeInterval,
                cboTimeInterval().SelectedIndex()
            );

            if (MainWindow::Get()->m_keepAwake)
                MainWindow::Get()->m_keepAwake->Reload();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
        
    VOID PageSettings::tglBatteryPowerOnToggled(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            if (tglBatteryPower().IsOn())
            {
                tglBatterySaver().IsOn(false);
                stcBatterySaver().IsEnabled(true);
                fntIcoBatteryPower().Glyph(GLYPH_BATTERYPOWER_ON);
            }
            else
            {
                tglBatterySaver().IsOn(false);
                stcBatterySaver().IsEnabled(false);
                fntIcoBatteryPower().Glyph(GLYPH_BATTERYPOWER_OFF);
            }

            Settings::Set(
                Settings::SettingType::BatteryPower,
                tglBatteryPower().IsOn()
            );

            if(MainWindow::Get()->m_powerSettings)
                MainWindow::Get()->m_powerSettings->SetOverlayScheme();
            if (MainWindow::Get()->m_keepAwake)
                MainWindow::Get()->m_keepAwake->Reload();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageSettings::tglBatterySaverOnToggled(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            if (tglBatterySaver().IsOn())
                fntIcoBatterySaver().Glyph(GLYPH_BATTERYSAVER_ON);
            else
                fntIcoBatterySaver().Glyph(GLYPH_BATTERYSAVER_OFF);

            Settings::Set(
                Settings::SettingType::BatterySaver,
                tglBatterySaver().IsOn()
            );

            if(MainWindow::Get()->m_powerSettings)
                MainWindow::Get()->m_powerSettings->SetOverlayScheme();
            if (MainWindow::Get()->m_keepAwake)
                MainWindow::Get()->m_keepAwake->Reload();
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////

    VOID PageSettings::listViewItemClick(
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

    VOID PageSettings::TimeIntervalFinished(VOID)
    {
        if (!m_bLoading)
            tglEnableWakey().IsOn(false);
    }

    ///////////////////////////////////////////////////////////////////////////////
}
