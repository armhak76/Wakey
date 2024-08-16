#pragma once

#include "HotKey.g.h"

namespace winrt::wakey::implementation
{
    struct HotKey : HotKeyT<HotKey>
    {
    private:
        ::winrt::Windows::Foundation::Collections::IMap<
            winrt::Windows::System::VirtualKey, winrt::hstring
        >  m_keySeq;
        
        ::winrt::Microsoft::UI::Xaml::Window m_window;

    public:
        HotKey(_In_::winrt::Microsoft::UI::Xaml::Window const& window);
        void Close(void);

        void Restore            (_In_ ::winrt::Microsoft::UI::Xaml::Controls::TextBox const& txt);
        void ScanPreviewKeyDown (_In_ ::winrt::Windows::System::VirtualKey key, _In_ ::winrt::Microsoft::UI::Xaml::Controls::TextBox const& txt);
        void ScanKeyUp          (_In_::winrt::Windows::System::VirtualKey key, _In_::winrt::Microsoft::UI::Xaml::Controls::TextBox const& txt);
        void ScanModKey         (_In_ ::winrt::Windows::System::VirtualKey key, _In_ ::winrt::hstring value);
        void ScanKeyDown        (_In_ ::winrt::Windows::System::VirtualKey key);       
        bool Register           (void);
        void UnRegister         (void);
    };
}

namespace winrt::wakey::factory_implementation
{
    struct HotKey : HotKeyT<HotKey, implementation::HotKey>
    {
    };
}
