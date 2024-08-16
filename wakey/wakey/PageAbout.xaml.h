#pragma once

#include "PageAbout.g.h"
#include "MainWindow.xaml.h"

namespace winrt::wakey::implementation
{
    struct PageAbout : PageAboutT<PageAbout>
    {
        PageAbout(VOID);

        VOID Loaded         (_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        VOID OnNavigatedTo  (_In_ winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args);
        VOID OnNavigatedFrom(_In_ winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args);

        winrt::fire_and_forget btnUpdateClick (_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        winrt::fire_and_forget btnRateClick   (_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        winrt::fire_and_forget btnProClick    (_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);

        winrt::Windows::Foundation::IAsyncAction ShowSimpleDialog   (_In_ winrt::hstring hstrMessage, _In_ winrt::hstring hstrFunction);
        winrt::Windows::Foundation::IAsyncAction ShowStoreDialog    (_In_ winrt::hstring hstrFunction);
    };
}

namespace winrt::wakey::factory_implementation
{
    struct PageAbout : PageAboutT<PageAbout, implementation::PageAbout>
    {
    };
}
