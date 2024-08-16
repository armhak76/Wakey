#pragma once

#include "CustomDialog.g.h"

namespace winrt::wakey::implementation
{
    struct CustomDialog : CustomDialogT<CustomDialog>
    {
        IconType m_iconType;
        winrt::hstring m_hstrText;

        CustomDialog(VOID) = delete;
        CustomDialog(
            _In_ IconType iconType,
            _In_ winrt::hstring const& hstrText
        )
            : m_iconType(iconType)
            , m_hstrText(hstrText)
        {
        }

        VOID Loaded(
            _In_ [[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender,
            _In_ [[maybe_unused]] winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
        );

        VOID Closed(
            _In_ [[maybe_unused]] winrt::Microsoft::UI::Xaml::Controls::ContentDialog const& sender,
            _In_ [[maybe_unused]] winrt::Microsoft::UI::Xaml::Controls::ContentDialogClosedEventArgs const& args
        );
    };
}

namespace winrt::wakey::factory_implementation
{
    struct CustomDialog : CustomDialogT<CustomDialog, implementation::CustomDialog>
    {
    };
}
