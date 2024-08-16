#include "pch.h"
#include "HotKey.h"
#if __has_include("HotKey.g.cpp")
#include "HotKey.g.cpp"
#endif

using namespace winrt;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

#define HOTKEY_ID   1

namespace winrt::wakey::implementation
{
    ///////////////////////////////////////////////////////////////////////////////

    HotKey::HotKey(_In_::winrt::Microsoft::UI::Xaml::Window const& window)
          : m_window(window)
    {   
        m_keySeq =
            winrt::single_threaded_map<
                winrt::Windows::System::VirtualKey, winrt::hstring
            >();
    }

    ///////////////////////////////////////////////////////////////////////////////

    void HotKey::Close(void)
    {
        m_keySeq.Clear();
        UnRegister();
    }

    ///////////////////////////////////////////////////////////////////////////////

    void HotKey::Restore(
        _In_ ::winrt::Microsoft::UI::Xaml::Controls::TextBox const& txt
    )
    {
        UINT uHotKey = Settings::Get(
            Settings::HotKey,
            (int32_t)winrt::Windows::System::VirtualKey::W
        );
        
        UINT uHotKeyMod = Settings::Get(
            Settings::HotKeyMod,
            (int32_t)(MOD_SHIFT | MOD_CONTROL)
        );

        if (uHotKeyMod > 0 && uHotKey > 0)
        {
            std::wstring wstrKey;
            if (uHotKeyMod & MOD_SHIFT)
                wstrKey += L"Shift + ";
            if (uHotKeyMod & MOD_CONTROL)
                wstrKey += L"Ctrl + ";
            if (uHotKeyMod & MOD_ALT)
                wstrKey += L"Alt + ";
            if (uHotKeyMod & MOD_WIN)
                wstrKey += L"Win + ";

            wstrKey += (wchar_t)MapVirtualKeyW(uHotKey, MAPVK_VK_TO_CHAR);
            txt.Text(wstrKey);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    bool HotKey::Register(void)
    {
        UINT uHotKey = Settings::Get(
            Settings::HotKey,
            (int32_t)winrt::Windows::System::VirtualKey::W
        );

        UINT uHotKeyMod = Settings::Get(
            Settings::HotKeyMod,
            (int32_t)(MOD_SHIFT | MOD_CONTROL)
        );

        if (uHotKey &&
            uHotKeyMod)
        {
            UnRegister();
            return ::RegisterHotKey(
                Wnd::GetHwnd(m_window), HOTKEY_ID, uHotKeyMod, uHotKey
            );
        }

        return FALSE;
    }

    ///////////////////////////////////////////////////////////////////////////////

    void HotKey::UnRegister(void)
    {
        UnregisterHotKey(
            Wnd::GetHwnd(m_window), HOTKEY_ID
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    void HotKey::ScanPreviewKeyDown(
        _In_ winrt::Windows::System::VirtualKey key,
        _In_::winrt::Microsoft::UI::Xaml::Controls::TextBox const& txt
    )
    {
        if (key == winrt::Windows::System::VirtualKey::Tab)
            return;

        if (txt.FocusState() == ::winrt::Microsoft::UI::Xaml::FocusState::Unfocused)
            return;

        txt.Text(L"");

        ScanModKey(winrt::Windows::System::VirtualKey::Shift,        L"Shift");
        ScanModKey(winrt::Windows::System::VirtualKey::Control,      L"Ctrl");
        ScanModKey(winrt::Windows::System::VirtualKey::Menu,         L"Alt");
        ScanModKey(winrt::Windows::System::VirtualKey::RightWindows, L"Win");
        ScanModKey(winrt::Windows::System::VirtualKey::LeftWindows,  L"Win");

        if (m_keySeq.Size() > 0)
        {
            std::wstring wstrKeySeq;
            UINT uKeyMod = 0;
            bool bClear = false;

            ScanKeyDown(key);

            for (winrt::Windows::Foundation::Collections::IKeyValuePair<winrt::Windows::System::VirtualKey, winrt::hstring> it : m_keySeq)
            {
                if (!wstrKeySeq.empty())
                    wstrKeySeq += L" + ";

                wstrKeySeq += it.Value();
                switch (it.Key())
                {
                case winrt::Windows::System::VirtualKey::Shift:
                    uKeyMod |= MOD_SHIFT;
                    break;
                case winrt::Windows::System::VirtualKey::Control:
                    uKeyMod |= MOD_CONTROL;
                    break;
                case winrt::Windows::System::VirtualKey::Menu:
                    uKeyMod |= MOD_ALT;
                    break;
                case winrt::Windows::System::VirtualKey::LeftWindows:
                case winrt::Windows::System::VirtualKey::RightWindows:
                    uKeyMod |= MOD_WIN;
                    break;
                default:
                {
                    if (it.Key() >= winrt::Windows::System::VirtualKey::A &&
                        it.Key() <= winrt::Windows::System::VirtualKey::Z)
                    {
                        txt.Text(wstrKeySeq);

                        Settings::Set(
                            Settings::HotKey,
                            (int32_t)MapVirtualKeyW((UINT)it.Key(), MAPVK_VK_TO_CHAR)
                        );
                        Settings::Set(
                            Settings::HotKeyMod, (int32_t)uKeyMod
                        );

                        Register();
                        bClear = true;
                    }
                }
                break;
                }
            }

            //if (bClear)
            //    m_keySeq.Clear();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    void HotKey::ScanModKey(
        _In_ winrt::Windows::System::VirtualKey key,
        _In_ winrt::hstring value
    )
    {
        winrt::Windows::UI::Core::CoreVirtualKeyStates keyState =
            winrt::Microsoft::UI::Input::InputKeyboardSource::GetKeyStateForCurrentThread(key);

        uint32_t keyDown =
            (uint32_t)winrt::Windows::UI::Core::CoreVirtualKeyStates::Down;

        if (((uint32_t)keyState & keyDown) == keyDown)
        {
            if (!m_keySeq.HasKey(key))
                m_keySeq.Insert(key, value);
        }
        else if (m_keySeq.HasKey(key))
            m_keySeq.Remove(key);
    }

    ///////////////////////////////////////////////////////////////////////////////

    void HotKey::ScanKeyDown(
        _In_::winrt::Windows::System::VirtualKey key
    )
    {
        if ((key >= winrt::Windows::System::VirtualKey::A   &&
             key <= winrt::Windows::System::VirtualKey::Z ) &&
            !m_keySeq.HasKey(key))
        {
            winrt::Windows::System::VirtualKey virtKey =
                winrt::Windows::System::VirtualKey::None;

            for (winrt::Windows::Foundation::Collections::IKeyValuePair<winrt::Windows::System::VirtualKey, winrt::hstring> it : m_keySeq)
            {
                switch (it.Key())
                {
                case winrt::Windows::System::VirtualKey::Shift:
                case winrt::Windows::System::VirtualKey::Control:
                case winrt::Windows::System::VirtualKey::Menu:
                case winrt::Windows::System::VirtualKey::LeftWindows:
                case winrt::Windows::System::VirtualKey::RightWindows:
                    virtKey =
                        winrt::Windows::System::VirtualKey::None;
                    break;
                default:
                    virtKey = it.Key();
                    break;
                }
            }

            if (virtKey != winrt::Windows::System::VirtualKey::None)
                m_keySeq.Remove(virtKey);

            std::wstring wstrKeySeq;
            wstrKeySeq += (wchar_t)MapVirtualKeyW((UINT)key, MAPVK_VK_TO_CHAR);

            m_keySeq.Insert(key, wstrKeySeq);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    void HotKey::ScanKeyUp(
        _In_::winrt::Windows::System::VirtualKey key,
        _In_::winrt::Microsoft::UI::Xaml::Controls::TextBox const& txt
    )
    {
        if (key == winrt::Windows::System::VirtualKey::Tab)
            return;

        if (txt.FocusState() == ::winrt::Microsoft::UI::Xaml::FocusState::Unfocused)
            return;

        if (m_keySeq.HasKey(key))
            m_keySeq.Remove(key);
    }

    ///////////////////////////////////////////////////////////////////////////////
}
