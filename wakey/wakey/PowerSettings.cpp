#include "pch.h"
#include "PowerSettings.h"
#if __has_include("PowerSettings.g.cpp")
#include "PowerSettings.g.cpp"
#endif

using namespace winrt;

///////////////////////////////////////////////////////////////////////////////
#include <initguid.h>
// https://stackoverflow.com/questions/61869347/control-windows-10s-power-mode-programmatically
// Better Battery      { 961CC777 - 2547 - 4F9D - 8174 - 7D86181b8A7A }	BetterBatteryLifeOverlay
DEFINE_GUID(GUID_BETTER_BATTERY, 0x961CC777, 0x2547, 0x4F9D, 0x81, 0x74, 0x7D, 0x86, 0x18, 0x1B, 0x8A, 0x7A);
// Better Performance  { 381B4222 - F694 - 41F0 - 9685 - FF5BB260DF2E }	Balanced
DEFINE_GUID(GUID_BALANCED, 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
// Best Performance    { DED574B5 - 45A0 - 4F42 - 8737 - 46345C09C238 }	MaxPerformanceOverlay
DEFINE_GUID(GUID_BETTER_PERFORMANCE, 0xDED574B5, 0x45A0, 0x4F42, 0x87, 0x37, 0x46, 0x34, 0x5C, 0x09, 0xC2, 0x38);

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::wakey::implementation
{
    ///////////////////////////////////////////////////////////////////////////////

    LPCGUID PowerSettings::s_guids[3] =
    {
        &GUID_BETTER_BATTERY,
        &GUID_BALANCED,
        &GUID_BETTER_PERFORMANCE,
    };

    ///////////////////////////////////////////////////////////////////////////////

    PowerSettings::PowerSettings(
        _In_::winrt::Microsoft::UI::Xaml::Window const& window
    )
        : m_window(window)
        , m_hPnPowerSource(NULL)
        , m_hPnEnergySaver(NULL)
        , m_pPnPersonality(NULL)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////

    void PowerSettings::Close(void)
    {
        UnRegister();
        UnSetOverlayScheme();
    }
    
    ///////////////////////////////////////////////////////////////////////////////


    void PowerSettings::Register(void)
    {
        HWND hWnd = Wnd::GetHwnd(m_window);
        WINRT_ASSERT(hWnd);

        m_hPnPowerSource = RegisterPowerSettingNotification(
            hWnd,
            &GUID_ACDC_POWER_SOURCE,
            DEVICE_NOTIFY_WINDOW_HANDLE
        );
        m_hPnEnergySaver = RegisterPowerSettingNotification(
            hWnd,
            &GUID_POWER_SAVING_STATUS,
            DEVICE_NOTIFY_WINDOW_HANDLE
        );
        PowerRegisterForEffectivePowerModeNotifications(
            EFFECTIVE_POWER_MODE_V1,
            &EffectivePowerModeCallback,
            (LPVOID)this,
            &m_pPnPersonality
        );

        WINRT_ASSERT(m_hPnPowerSource);
        WINRT_ASSERT(m_hPnEnergySaver);
        WINRT_ASSERT(m_pPnPersonality);
    }

    ///////////////////////////////////////////////////////////////////////////////

    void PowerSettings::UnRegister(void)
    {
        WINRT_ASSERT(m_hPnPowerSource);
        WINRT_ASSERT(m_hPnEnergySaver);
        WINRT_ASSERT(m_pPnPersonality);

        if(m_hPnPowerSource)
            UnregisterPowerSettingNotification(m_hPnPowerSource);
        if(m_hPnEnergySaver)
            UnregisterPowerSettingNotification(m_hPnEnergySaver);
        if (m_pPnPersonality)
            PowerUnregisterFromEffectivePowerModeNotifications(m_pPnPersonality);
    }

    ///////////////////////////////////////////////////////////////////////////////

    void PowerSettings::SetOverlayScheme(void)
    {
        bool bKeepAwake =
            Settings::Get(Settings::SettingType::KeepAwake, false);

        bool bSetPowerOverlay = false;
        if (bKeepAwake)
        {
            SYSTEM_POWER_STATUS sps;
            SecureZeroMemory(&sps, sizeof(sps));
            ::GetSystemPowerStatus(&sps);

            bSetPowerOverlay =
                Settings::Get(Settings::SettingType::PowerOverlay, false);
            bool bRunOnBattery =
                Settings::Get(Settings::SettingType::BatteryPower, false);
            bool bRunOnBatSaver =
                Settings::Get(Settings::SettingType::BatterySaver, false);

            bool bPluggedIn = sps.ACLineStatus & AC_LINE_ONLINE;
            bool bEnergySaver = sps.SystemStatusFlag;

            if (bSetPowerOverlay)
            {
                if (!bRunOnBattery && !bPluggedIn)
                    bSetPowerOverlay = false;
                if (!bRunOnBatSaver && bEnergySaver)
                    bSetPowerOverlay = false;
            }
        }

        //PowerSetActiveScheme(0, &GUID_TYPICAL_POWER_SAVINGS);
        if (bSetPowerOverlay)
        {
            SetActiveOverlayScheme(
                OverlayScheme::BestPerformance
            );
        }
        else
        {
            UnSetOverlayScheme();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    void PowerSettings::UnSetOverlayScheme(void)
    {
        SetActiveOverlayScheme(
            (OverlayScheme)Settings::Get(
                Settings::SettingType::PowerOverlayScheme,
                (int32_t)OverlayScheme::Balanced
            )
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    void PowerSettings::GetOverlayScheme(void)
    {
        OverlayScheme pos = OverlayScheme::Balanced;
        GetEffectiveOverlayScheme(&pos);
        Settings::Set(
            Settings::SettingType::PowerOverlayScheme, (int32_t)pos
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    bool PowerSettings::IsBatteryPresent(void)
    {
        SYSTEM_POWER_STATUS sps;
        SecureZeroMemory(&sps, sizeof(sps));

        ::GetSystemPowerStatus(&sps);
        return sps.BatteryFlag < 128;
    }
    
    ///////////////////////////////////////////////////////////////////////////////

    bool PowerSettings::GetEffectiveOverlayScheme(_Out_ OverlayScheme* pos)
    {
        WINRT_ASSERT(pos);
        if (!pos)
            return FALSE;

        *pos = OverlayScheme::Invalid;

        GUID guidPOS;
        SecureZeroMemory(&guidPOS, sizeof(GUID));

        bool bRetval = GetEffectiveOverlayScheme(&guidPOS);
        if (bRetval)
        {
            for (UINT i = 0; i < __crt_countof(s_guids); i++)
            {
                if (RtlCompareMemory(s_guids[i], &guidPOS, sizeof(GUID)) == sizeof(GUID))
                {
                    *pos = (OverlayScheme)i;
                    break;
                }
            }
        }

        return bRetval;
    }

    ///////////////////////////////////////////////////////////////////////////////

    bool PowerSettings::SetActiveOverlayScheme(_In_ OverlayScheme pos)
    {
        WINRT_ASSERT(pos != OverlayScheme::Invalid);
        if (pos == OverlayScheme::Invalid)
            return false;

        return SetActiveOverlayScheme(s_guids[pos]);
    }

    ///////////////////////////////////////////////////////////////////////////////

    bool PowerSettings::GetEffectiveOverlayScheme(_Out_ LPGUID pGUID)
    {
        bool bRetval = false;
        if (pGUID)
        {
            SecureZeroMemory(pGUID, sizeof(GUID));

            HMODULE hPwrLib = LoadLibraryW(L"powrprof.dll");
            if (hPwrLib)
            {
                typedef DWORD(WINAPI* LPPOWERGETACTIVEOVERLAYSCHEME) (
                    _Out_ LPGUID pGUID
                    );

                LPPOWERGETACTIVEOVERLAYSCHEME pfnPowerGetEffectiveOverlayScheme =
                    (LPPOWERGETACTIVEOVERLAYSCHEME)GetProcAddress(
                        hPwrLib, "PowerGetEffectiveOverlayScheme"
                    );

                if (pfnPowerGetEffectiveOverlayScheme)
                    bRetval = !pfnPowerGetEffectiveOverlayScheme(pGUID);

                FreeLibrary(hPwrLib);
            }
        }

        return bRetval;
    }

    ///////////////////////////////////////////////////////////////////////////////

    bool PowerSettings::SetActiveOverlayScheme(_In_ LPCGUID pGUID)
    {
        bool bRetval = FALSE;
        if (pGUID)
        {
            HMODULE hPwrLib = LoadLibraryW(L"powrprof.dll");
            if (hPwrLib)
            {
                typedef DWORD(WINAPI* LPPOWERSETACTIVEOVERLAYSCHEME) (
                    _In_ LPCGUID pGUID
                    );

                LPPOWERSETACTIVEOVERLAYSCHEME pfnPowerSetActiveOverlayScheme =
                    (LPPOWERSETACTIVEOVERLAYSCHEME)GetProcAddress(
                        hPwrLib, "PowerSetActiveOverlayScheme"
                    );

                if (pfnPowerSetActiveOverlayScheme)
                    bRetval = !pfnPowerSetActiveOverlayScheme(pGUID);

                FreeLibrary(hPwrLib);
            }
        }

        return bRetval;
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID WINAPI PowerSettings::EffectivePowerModeCallback(
        _In_ EFFECTIVE_POWER_MODE eMode,
        _In_opt_ VOID* pContext)
    {
        if (eMode != EFFECTIVE_POWER_MODE::EffectivePowerModeHighPerformance)
        {
            PowerSettings* pThis = (PowerSettings*)pContext;
            pThis->SetOverlayScheme();
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
}
