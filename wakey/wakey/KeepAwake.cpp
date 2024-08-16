#include "pch.h"
#include "KeepAwake.h"
#if __has_include("KeepAwake.g.cpp")
#include "KeepAwake.g.cpp"
#endif

using namespace winrt;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::wakey::implementation
{
    ///////////////////////////////////////////////////////////////////////////////

    KeepAwake::KeepAwake(
        _In_ winrt::Microsoft::UI::Xaml::Window window,
        _In_ UINT uMsgTimeOut
    )
        : m_hThread    (NULL)
        , m_hWnd       (Wnd::GetHwnd(window))
        , m_hReload    (NULL)
        , m_llTimeOut  (0)
        , m_bFirst     (FALSE)
        , m_uMsgTimeOut(uMsgTimeOut)
    {
        WINRT_ASSERT(m_hWnd);
        WINRT_ASSERT(m_uMsgTimeOut);
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID KeepAwake::StatusUpdate(VOID)
    {
        if (!Notifications::ShouldShow())
            return;

        bool bKeepAwake = 
            Settings::Get(Settings::SettingType::KeepAwake, false);

        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader resLoader;

        std::wstring wstrNotification;
        wstrNotification += resLoader.GetString(L"resAppTitle");
        wstrNotification += L" ";
        if (bKeepAwake)
        {
            int32_t iTimeInterval =
                Settings::Get(Settings::SettingType::TimeInterval, 0);
            
            wstrNotification += resLoader.GetString(L"resStatusUpdateIsActive");
            if (iTimeInterval != 0)
            {
                wstrNotification += L" ";
                wstrNotification += resLoader.GetString(L"resStatusUpdateFor");
                wstrNotification += L" ";
                wstrNotification += std::to_wstring(iTimeInterval);
                wstrNotification += L" ";
                wstrNotification += resLoader.GetString(L"resStatusUpdateHours");                
            }
            wstrNotification += L" ";
            wstrNotification += resLoader.GetString(L"resStatusUpdateRunning");
        }
        else
        {
            wstrNotification += resLoader.GetString(L"resStatusUpdateIsInactive");
        }

        Notifications::Show(
            resLoader.GetString(L"resStatusUpdate"), wstrNotification.c_str()
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID CALLBACK KeepAwake::APCProc(_In_ ULONG_PTR Parameter)
    {
        UNREFERENCED_PARAMETER(Parameter);
    }

    ///////////////////////////////////////////////////////////////////////////////

    DWORD CALLBACK KeepAwake::ThreadProc(_In_ LPVOID pParam)
    {
        UNREFERENCED_PARAMETER(pParam);

        DWORD dwFlags   = 0;
        DWORD dwWait    = WAIT_FAILED;        
                
        SYSTEM_POWER_STATUS sps;

        bool bKeepAwake    = false;
        bool bKeepScreenOn = false;
        int32_t iTimeInterval = 0;
        bool bRunOnBattery = false;
        bool bRunOnBatSaver = false;

        KeepAwake* pThis = (KeepAwake*)pParam;
        WINRT_ASSERT(pThis);
        if (!pThis)
            return (DWORD)-1;

        HANDLE hTimerDelay   = CreateWaitableTimer(NULL, FALSE, NULL);
        HANDLE hTimerTimeOut = CreateWaitableTimer(NULL, FALSE, NULL);

        WINRT_ASSERT(hTimerDelay);
        WINRT_ASSERT(hTimerTimeOut);

        LARGE_INTEGER liDueTime;
        liDueTime.QuadPart = -50000000LL;

        LARGE_INTEGER liWaitTimeOut;
        liWaitTimeOut.QuadPart = 0;

        BOOL bWait  = FALSE;
        BOOL bBreak = FALSE;

        BOOL bPluggedIn   = FALSE;
        BOOL bEnergySaver = FALSE;

        HANDLE aWait[] = { 
            pThis->m_hReload, hTimerDelay, hTimerTimeOut
        };

        do
        {
            dwFlags = ES_CONTINUOUS;

            bKeepAwake =
                Settings::Get(Settings::SettingType::KeepAwake, false);
            
            if (bKeepAwake)
            {
                bKeepScreenOn =
                    Settings::Get(Settings::SettingType::KeepScreenOn, false);
                iTimeInterval =
                    Settings::Get(Settings::SettingType::TimeInterval, 0);
                bRunOnBattery =
                    Settings::Get(Settings::SettingType::BatteryPower, false);
                bRunOnBatSaver =
                    Settings::Get(Settings::SettingType::BatterySaver, false);
                                        
                SecureZeroMemory(&liWaitTimeOut, sizeof(liWaitTimeOut));
                if(iTimeInterval)
                {
                    liWaitTimeOut.QuadPart = iTimeInterval;
                #ifndef _DEBUG
                    liWaitTimeOut.QuadPart *= (60 * 60 * -10000000LL);
                #else
                    liWaitTimeOut.QuadPart *= -10000000LL;
                #endif
                    ::SetWaitableTimer(
                        hTimerTimeOut, &liWaitTimeOut, 0, NULL, NULL, 0
                    );
                }

                SecureZeroMemory(&sps, sizeof(sps));
                ::GetSystemPowerStatus(&sps);

                bPluggedIn   = sps.ACLineStatus & AC_LINE_ONLINE;
                bEnergySaver = sps.SystemStatusFlag;

                if (bPluggedIn)
                {
                    dwFlags |= ES_SYSTEM_REQUIRED;
                }
                else if (bRunOnBattery)
                {
                    dwFlags |= ES_SYSTEM_REQUIRED;

                    if(bEnergySaver && !bRunOnBatSaver)
                        dwFlags &= ~ES_SYSTEM_REQUIRED;
                }
                
                if (bKeepScreenOn && (dwFlags & ES_SYSTEM_REQUIRED))
                    dwFlags |= ES_DISPLAY_REQUIRED;    
            }

            if (!pThis->m_bFirst)
                ::SetWaitableTimer(hTimerDelay, &liDueTime, 0, NULL, NULL, 0);

            pThis->m_bFirst = FALSE;

            ::SetThreadExecutionState(dwFlags);
            do
            {
                dwWait = ::WaitForMultipleObjectsEx(
                    __crt_countof(aWait), aWait, FALSE, INFINITE, TRUE
                );

                if (dwWait == WAIT_OBJECT_0)
                {
                    bWait = FALSE;

                    ::SetWaitableTimer(
                        hTimerDelay, &liDueTime, 0, NULL, NULL, 0
                    );
                }
                else if (dwWait == WAIT_OBJECT_0 + 1)
                {
                    bWait = TRUE;
                    pThis->StatusUpdate();
                }
                else if (dwWait == WAIT_OBJECT_0 + 2)
                {
                    bWait = TRUE;
                    pThis->m_llTimeOut = 0;

                    Settings::Set(Settings::SettingType::KeepAwake,    false);
                    Settings::Set(Settings::SettingType::KeepScreenOn, false);
                    Settings::Set(Settings::SettingType::TimeInterval, 0);
                    Settings::Set(Settings::SettingType::BatteryPower, false);
                    Settings::Set(Settings::SettingType::BatterySaver, false);
                    Settings::Set(Settings::SettingType::PowerOverlay, false);
                    Settings::Set(Settings::SettingType::Bluetooth,    false);

                    SendMessage(
                        pThis->m_hWnd, pThis->m_uMsgTimeOut, 0, 0
                    );

                    pThis->StatusUpdate();
                }
                else
                {
                    bBreak = TRUE;
                }
            }
            while (bWait && !bBreak);
        }
        while (!bBreak);

        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID KeepAwake::Start(_In_ BOOL bFirst)
    {
        Stop();
        m_bFirst = bFirst;

        WINRT_ASSERT(!m_hReload);
        m_hReload = CreateEventW(NULL, FALSE, FALSE, NULL);
        WINRT_ASSERT(m_hReload);

        WINRT_ASSERT(!m_hThread);
        m_hThread = CreateThread(
            NULL, 0, &ThreadProc, (LPVOID)this, 0, NULL
        );
        WINRT_ASSERT(m_hThread);
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID KeepAwake::Reload(VOID)
    {
        WINRT_ASSERT(m_hThread);
        WINRT_ASSERT(m_hReload);

        if(m_hThread && m_hReload)
            ::SetEvent(m_hReload);
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID KeepAwake::Stop(VOID)
    {
        //WINRT_ASSERT(m_hThread);
        if (m_hThread)
        {
            ::QueueUserAPC(&APCProc, m_hThread, 0);

            if (WAIT_OBJECT_0 != ::WaitForMultipleObjectsEx(1, &m_hThread, FALSE, 500, FALSE))
#pragma warning( push )
#pragma warning( disable : 6258)
                ::TerminateThread(m_hThread, (DWORD)-1);
#pragma warning( pop )
            ::CloseHandle(m_hThread);
            m_hThread = NULL;
        }

        //WINRT_ASSERT(m_hReload);
        if (m_hReload)
        {
            ::CloseHandle(m_hReload);
            m_hReload = NULL;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID KeepAwake::Close(VOID)
    {
        Stop();
    }

    ///////////////////////////////////////////////////////////////////////////////
}
