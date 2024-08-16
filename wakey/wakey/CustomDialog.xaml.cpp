#include "pch.h"
#include "CustomDialog.xaml.h"
#if __has_include("CustomDialog.g.cpp")
#include "CustomDialog.g.cpp"
#endif

using namespace winrt;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::wakey::implementation
{
    ///////////////////////////////////////////////////////////////////////////////

    VOID CustomDialog::Loaded(
        _In_ winrt::Windows::Foundation::IInspectable const& sender,
        _In_ winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        static winrt::hstring hstrIcons[] = {
            L"\uE946", L"\uE7BA", L"\uEA39", L"\uE9CE", L"\uE8C7"
        };

        winrt::Windows::ApplicationModel::Package curPackage =
            ::winrt::Windows::ApplicationModel::Package::Current();

        Icon().Glyph(hstrIcons[(int32_t)m_iconType].c_str());
        Title(winrt::box_value(curPackage.DisplayName()));
        Text().Text(m_hstrText);
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID CustomDialog::Closed(
        _In_ winrt::Microsoft::UI::Xaml::Controls::ContentDialog const& sender,
        _In_ winrt::Microsoft::UI::Xaml::Controls::ContentDialogClosedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);
    }

    ///////////////////////////////////////////////////////////////////////////////
}
