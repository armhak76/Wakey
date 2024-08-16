#pragma once

#include "PageBluetooth.g.h"

namespace winrt::wakey::implementation
{
    struct PageBluetooth : PageBluetoothT<PageBluetooth>
    {
        static winrt::hstring s_hstrDevIdNone;

        BOOL m_bLoading;
        BOOL m_bBtWin;

        PageBluetooth(VOID);

        VOID Loaded                 (_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        VOID OnNavigatedTo          (_In_ winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args);
        VOID OnNavigatedFrom        (_In_ winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args);
        VOID cboLinkSelChanged      (_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& args);
        VOID tglBluetoothOnToggled  (_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        VOID btnLinkBluetoothClick  (_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        VOID btnDynamicLockClick    (_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        VOID listViewItemClick      (_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::Controls::ItemClickEventArgs const& args);
        VOID tglDynamicLockOnToggled(_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        VOID btnBluetoothWinClick   (_In_::winrt::Windows::Foundation::IInspectable const& sender, _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);

        winrt::fire_and_forget MonitorDynamicLock       (VOID);

        winrt::fire_and_forget OnBluetoothStatusChanged (_In_ BluetoothStatus eBtStatus);
        winrt::fire_and_forget OnBluetoothDeviceChanged (_In_ winrt::hstring hstrId, _In_ BluetoothDeviceChange eBtDeviceChange);
        winrt::event_token m_etDevChng;
        winrt::event_token m_etBtState;
    };
}

namespace winrt::wakey::factory_implementation
{
    struct PageBluetooth : PageBluetoothT<PageBluetooth, implementation::PageBluetooth>
    {
    };
}
