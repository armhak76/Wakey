#pragma once

#include "PageGeneral.g.h"

namespace winrt::wakey::implementation
{
    struct PageGeneral : PageGeneralT<PageGeneral>
    {
        BOOL m_bLoading;
        PageGeneral(VOID);

        ::winrt::fire_and_forget AutorunGetFAF(VOID);

        ::winrt::Windows::Foundation::IAsyncAction AutorunEnableAsync(VOID);        
        ::winrt::Windows::Foundation::IAsyncAction AutorunGetAsync   (VOID);

        VOID Loaded                      (_In_ ::winrt::Windows::Foundation::IInspectable const& sender, _In_ ::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        VOID OnNavigatedTo               (_In_ winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args);
        VOID OnNavigatedFrom             (_In_ winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args);
        VOID PreviewKeyDown              (_In_ ::winrt::Windows::Foundation::IInspectable const& sender, _In_ ::winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& args);
        VOID KeyUp                       (_In_ ::winrt::Windows::Foundation::IInspectable const& sender, _In_ ::winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& args);
        VOID tglNotificationsOnToggled   (_In_ ::winrt::Windows::Foundation::IInspectable const& sender, _In_ ::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);        
        VOID listViewItemClick           (_In_ ::winrt::Windows::Foundation::IInspectable const& sender, _In_ ::winrt::Microsoft::UI::Xaml::Controls::ItemClickEventArgs const& args);
        VOID txtHotKeyTextChanged        (_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args);
        VOID btnRunAsAdminClicked        (_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        VOID tglRunAsAdminAlwaysOnToggled(_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);

        ::winrt::fire_and_forget tglAutorunOnToggled(_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        ::winrt::fire_and_forget cboThemeSelChanged(_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& args);
    };
}

namespace winrt::wakey::factory_implementation
{
    struct PageGeneral : PageGeneralT<PageGeneral, implementation::PageGeneral>
    {
    };
}
