#pragma once

#include "PowerSettings.g.h"

//#define NTDDI_VERSION   NTDDI_WIN10_RS5   //0x0A000006 1809
//#define _WIN32_WINNT    _WIN32_WINNT_WIN10 //Windows 10 (0x0A00)
#include <PowrProf.h>
#pragma comment(lib, "PowrProf.lib")

namespace winrt::wakey::implementation
{
    struct PowerSettings : PowerSettingsT<PowerSettings>
    {
    public:
        enum OverlayScheme
        {
            BestPowerEfficiency,
            Balanced,
            BestPerformance,
            Invalid,
        };

    private:
        static LPCGUID s_guids[3];

        HPOWERNOTIFY m_hPnPowerSource;
        HPOWERNOTIFY m_hPnEnergySaver;
        LPVOID       m_pPnPersonality;

        ::winrt::Microsoft::UI::Xaml::Window m_window;

    public:
        PowerSettings           (_In_::winrt::Microsoft::UI::Xaml::Window const& window);

        void Register           (void);
        void UnRegister         (void);
        void Close              (void);

        void GetOverlayScheme   (void);
        void SetOverlayScheme   (void);
        void UnSetOverlayScheme (void);

        bool IsBatteryPresent   (void);

    private:
        static bool GetEffectiveOverlayScheme   (_Out_ OverlayScheme* pos);
        static bool SetActiveOverlayScheme      (_In_  OverlayScheme pos);
        static bool GetEffectiveOverlayScheme   (_Out_ LPGUID pGUID);
        static bool SetActiveOverlayScheme      (_In_ LPCGUID pGUID);

        static VOID WINAPI EffectivePowerModeCallback(_In_ EFFECTIVE_POWER_MODE eMode, _In_opt_ VOID* pContext);
    };
}

namespace winrt::wakey::factory_implementation
{
    struct PowerSettings : PowerSettingsT<PowerSettings, implementation::PowerSettings>
    {
    };
}
