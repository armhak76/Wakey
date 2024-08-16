#pragma once

#include "PageSettings.g.h"

namespace winrt::wakey::implementation
{
    struct PageSettings : PageSettingsT<PageSettings>
    {
        BOOL m_bLoading;
        PageSettings(VOID);

        VOID Loaded                     (_In_ ::winrt::Windows::Foundation::IInspectable const& sender, _In_ ::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        VOID OnNavigatedTo              (_In_ winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args);
        VOID OnNavigatedFrom            (_In_ winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args);

        
        VOID tglEnableWakeyOnToggled    (_In_ ::winrt::Windows::Foundation::IInspectable const& sender, _In_ ::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        VOID tglKeepScreenOnToggled     (_In_ ::winrt::Windows::Foundation::IInspectable const& sender, _In_ ::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        VOID tglPowerModeOnToggled      (_In_ ::winrt::Windows::Foundation::IInspectable const& sender, _In_ ::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        VOID btnPowerSettingsClick      (_In_ ::winrt::Windows::Foundation::IInspectable const& sender, _In_ ::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        VOID cboTimeIntervalSelChanged  (_In_ ::winrt::Windows::Foundation::IInspectable const& sender, _In_ ::winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& args);
        VOID tglBatteryPowerOnToggled   (_In_ ::winrt::Windows::Foundation::IInspectable const& sender, _In_ ::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        VOID tglBatterySaverOnToggled   (_In_ ::winrt::Windows::Foundation::IInspectable const& sender, _In_ ::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        VOID listViewItemClick          (_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::Controls::ItemClickEventArgs const& args);

        VOID TimeIntervalFinished       (VOID);
    };
}

namespace winrt::wakey::factory_implementation
{
    struct PageSettings : PageSettingsT<PageSettings, implementation::PageSettings>
    {
    };
}