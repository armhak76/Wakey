#pragma once

#include "App.xaml.g.h"

namespace winrt::wakey::implementation
{
    struct App : AppT<App>
    {
        App(VOID);

        VOID OnLaunched(_In_ winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const& e);
        VOID Exit(VOID);
        
        winrt::fire_and_forget RedirectActivationFAF(_In_ winrt::Microsoft::Windows::AppLifecycle::AppInstance const& currentInstance, _In_ winrt::Microsoft::Windows::AppLifecycle::AppActivationArguments const& activationArgs);   
        VOID OnActivated (_In_ winrt::Windows::Foundation::IInspectable const& sender, _In_ winrt::Microsoft::Windows::AppLifecycle::AppActivationArguments const& args);
        
    public:
        static App* s_pThis;
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
    };    
}