#include "pch.h"
#include "PageGeneral.xaml.h"
#if __has_include("PageGeneral.g.cpp")
    #include "PageGeneral.g.cpp"
#endif

#include "App.xaml.h"
#include "CustomDialog.xaml.h"
#include "MainWindow.xaml.h"

using namespace winrt;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

#define GLYPH_AUTORUN_ON        L"\uE718"
#define GLYPH_AUTORUN_OFF       L"\uE77A"
#define GLYPH_NOTIFY_ON         L"\uE91C"
#define GLYPH_NOTIFY_OFF        L"\uEE79"
#define GLYPH_THEME_ON          L"\uE790"
#define GLYPH_THEME_OFF         L"\uF570"
#define GLYPH_HOTKEY_ON         L"\uEDA7"
#define GLYPH_HOTKEY_OFF        L"\uE765"
#define GLYPH_AUTORUN_ON        L"\uE718"
#define GLYPH_AUTORUN_OFF       L"\uE77A"
#define GLYPH_RUNASADMIN_ON     L"\uEA18"
#define GLYPH_RUNASADMIN_OFF    L"\uE83D"

namespace winrt::wakey::implementation
{
    ///////////////////////////////////////////////////////////////////////////////

    PageGeneral::PageGeneral(VOID)
        : m_bLoading(TRUE)
    {
        // Xaml objects should not call InitializeComponent during construction.
        // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent

        NavigationCacheMode(
            winrt::Microsoft::UI::Xaml::Navigation::NavigationCacheMode::Enabled
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageGeneral::Loaded(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (m_bLoading)
        {
            MainWindow::Get()->m_hotKey->Restore(txtHotKey());
            if (!txtHotKey().Text().empty())
                fntIcoHotKey().Glyph(GLYPH_HOTKEY_ON);
            else
                fntIcoHotKey().Glyph(GLYPH_HOTKEY_OFF);

            int32_t iThemeSelection = 
                Settings::Get(Settings::SettingType::Theme, -1);

            cboTheme().SelectedIndex(iThemeSelection + 1);

            tglNotifications().IsOn(
                Settings::Get(Settings::SettingType::Notifications, false)
            );
            if (tglNotifications().IsOn())
                fntIcoNotifications().Glyph(GLYPH_NOTIFY_ON);
            else
                fntIcoNotifications().Glyph(GLYPH_NOTIFY_OFF);

            AutorunGetFAF();
            if (Program::IsRunAsAdmin())
            {
                fntIcoAdmin().Glyph(GLYPH_RUNASADMIN_ON);
                stcRunAsAdmin().IsEnabled(false);
            }
            else
            {
                fntIcoAdmin().Glyph(GLYPH_RUNASADMIN_OFF);
                stcRunAsAdmin().IsEnabled(true);
            }

            if (Settings::Get(Settings::SideLoaded, false))
            {
                txtRunAsAdmin().Visibility(
                    winrt::Microsoft::UI::Xaml::Visibility::Visible
                );
                steRunAsAdmin().Visibility(
                    winrt::Microsoft::UI::Xaml::Visibility::Visible
                );

                if (Program::IsRunAsAdmin())
                {
                    fntIcoAdminAlways  ().Glyph(GLYPH_RUNASADMIN_ON);
                    stcRunAsAdminAlways().IsEnabled(true);
                    tglRunAsAdminAlways().IsEnabled(true);
                    tglRunAsAdminAlways().IsOn(
                        Settings::Get(Settings::RunAsAdminAlways, false)
                    );
                }
                else
                {
                    tglRunAsAdminAlways().IsEnabled(false);
                    tglRunAsAdminAlways().IsOn(false);
                    fntIcoAdminAlways  ().Glyph(GLYPH_RUNASADMIN_OFF);
                    stcRunAsAdminAlways().IsEnabled(false);
                }
            }
            else
            {
                txtRunAsAdmin().Visibility(
                    winrt::Microsoft::UI::Xaml::Visibility::Collapsed
                );
                steRunAsAdmin().Visibility(
                    winrt::Microsoft::UI::Xaml::Visibility::Collapsed
                );
            }

            if (!txtHotKey().Text().empty())
                fntIcoHotKey().Glyph(GLYPH_HOTKEY_ON);
            else
                fntIcoHotKey().Glyph(GLYPH_HOTKEY_OFF);

            if (iThemeSelection != -1)
                fntIcoTheme().Glyph(GLYPH_THEME_ON);
            else
                fntIcoTheme().Glyph(GLYPH_THEME_OFF);

            m_bLoading = FALSE;
        }        
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageGeneral::OnNavigatedTo(
        _In_ winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(args);
        if (!m_bLoading)
        {
            if (Settings::Get(Settings::SideLoaded, false))
            {
                txtRunAsAdmin().Visibility(
                    winrt::Microsoft::UI::Xaml::Visibility::Visible
                );
                steRunAsAdmin().Visibility(
                    winrt::Microsoft::UI::Xaml::Visibility::Visible
                );

                if (Program::IsRunAsAdmin())
                {
                    stcRunAsAdmin().IsEnabled(false);
                    fntIcoAdminAlways().Glyph(GLYPH_RUNASADMIN_ON);
                    stcRunAsAdminAlways().IsEnabled(true);
                    tglRunAsAdminAlways().IsEnabled(true);
                    tglRunAsAdminAlways().IsOn(
                        Settings::Get(Settings::RunAsAdminAlways, false)
                    );
                }
                else
                {
                    stcRunAsAdmin().IsEnabled(true);
                    tglRunAsAdminAlways().IsEnabled(false);
                    tglRunAsAdminAlways().IsOn(false);
                    fntIcoAdminAlways().Glyph(GLYPH_RUNASADMIN_OFF);
                    stcRunAsAdminAlways().IsEnabled(false);
                }
            }
            else
            {
                txtRunAsAdmin().Visibility(
                    winrt::Microsoft::UI::Xaml::Visibility::Collapsed
                );
                steRunAsAdmin().Visibility(
                    winrt::Microsoft::UI::Xaml::Visibility::Collapsed
                );
            }
        }        
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageGeneral::OnNavigatedFrom(
        _In_ winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(args);
    }

    ///////////////////////////////////////////////////////////////////////////////

    ::winrt::fire_and_forget PageGeneral::tglAutorunOnToggled(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
            co_await AutorunEnableAsync();
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageGeneral::tglNotificationsOnToggled(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            if (tglNotifications().IsOn())
                fntIcoNotifications().Glyph(GLYPH_NOTIFY_ON);
            else
                fntIcoNotifications().Glyph(GLYPH_NOTIFY_OFF);

            Settings::Set(
                Settings::SettingType::Notifications,
                tglNotifications().IsOn()
            );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageGeneral::PreviewKeyDown(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        MainWindow::Get()->m_hotKey->ScanPreviewKeyDown(args.Key(), txtHotKey());
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageGeneral::KeyUp(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        MainWindow::Get()->m_hotKey->ScanKeyUp(args.Key(), txtHotKey());
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageGeneral::txtHotKeyTextChanged(
       _In_::winrt::Windows::Foundation::IInspectable const& sender,
       _In_::winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            if (!txtHotKey().Text().empty())
                fntIcoHotKey().Glyph(GLYPH_HOTKEY_ON);
            else
                fntIcoHotKey().Glyph(GLYPH_HOTKEY_OFF);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageGeneral::btnRunAsAdminClicked(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            if (Settings::Get(Settings::SideLoaded, false))
            {
                if (!Program::IsRunAsAdmin())
                {
                    Program::Restart(
                        MainWindow::Get()->try_as<::winrt::Microsoft::UI::Xaml::Window>(),
                        L"--runasonce"
                    );
                    return;
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    ::winrt::fire_and_forget PageGeneral::cboThemeSelChanged(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            int32_t iThemeSelection = cboTheme().SelectedIndex() - 1;
            if(iThemeSelection != -1)
                fntIcoTheme().Glyph(GLYPH_THEME_ON);
            else
                fntIcoTheme().Glyph(GLYPH_THEME_OFF);

            Settings::Set(
                Settings::SettingType::Theme, iThemeSelection
            );

            winrt::Microsoft::UI::Xaml::ApplicationTheme appThemeSysDefault =
                Theme::GetSystemDefault();

            winrt::Microsoft::UI::Xaml::ApplicationTheme appThemeAppCurrent = 
                ::winrt::Microsoft::UI::Xaml::Application::Current().RequestedTheme();

            winrt::Microsoft::UI::Xaml::ApplicationTheme appThemeSelection;
            if (iThemeSelection == -1)
                appThemeSelection = appThemeSysDefault;
            else
                appThemeSelection = (winrt::Microsoft::UI::Xaml::ApplicationTheme)iThemeSelection;

            if (appThemeSelection != appThemeSysDefault ||
                appThemeSelection != appThemeAppCurrent)
            {
                winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader resLoader;
               
                ::winrt::Microsoft::UI::Xaml::Controls::ContentDialog customDialog;
                customDialog = winrt::make<
                    winrt::wakey::implementation::CustomDialog>(
                        winrt::wakey::IconType::Question, resLoader.GetString(L"resAppThemeMsg")
                    );

                customDialog.XamlRoot(
                    MainWindow::Get()->Content().XamlRoot()
                );
                customDialog.PrimaryButtonText(L"Yes");
                customDialog.DefaultButton(
                    winrt::Microsoft::UI::Xaml::Controls::ContentDialogButton::Primary
                );
                customDialog.CloseButtonText(L"No");
                customDialog.PrimaryButtonClick([&](auto&& ...)
                    {
                        Program::Restart(
                            MainWindow::Get()->try_as<::winrt::Microsoft::UI::Xaml::Window>(),
                            L"--theme"
                        );
                    });
                co_await MainWindow::Get()->ShowDialog(customDialog, __FUNCTIONW__);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageGeneral::listViewItemClick(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::Controls::ItemClickEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);

        Mntone::AngelUmbrella::Controls::SettingsPanel sp =
            args.ClickedItem().try_as<Mntone::AngelUmbrella::Controls::SettingsPanel>();

        MainWindow::Get()->GoToPage(sp.Name(), false, true);
    }

    ///////////////////////////////////////////////////////////////////////////////
    
    ::winrt::Windows::Foundation::IAsyncAction PageGeneral::AutorunGetAsync(VOID)
    {
        winrt::apartment_context ui_thread; // Capture calling context.
        prgAutorun().IsActive(true);

        co_await winrt::resume_background();
        // Do compute-bound work here.

        ::winrt::Windows::ApplicationModel::StartupTask startupTask =
            co_await ::winrt::Windows::ApplicationModel::StartupTask::GetAsync(L"Wakey");

        co_await ui_thread; // Switch back to calling context.
        prgAutorun().IsActive(false);

        bool bAutorunEnabled =
            startupTask.State() == ::winrt::Windows::ApplicationModel::StartupTaskState::Enabled;

        tglAutorun().IsOn(bAutorunEnabled);
        Settings::Set(
            Settings::SettingType::Autorun,
            bAutorunEnabled
        );
        
        if (bAutorunEnabled)
            fntIcoAutorun().Glyph(GLYPH_AUTORUN_ON);
        else
            fntIcoAutorun().Glyph(GLYPH_AUTORUN_OFF);
    }

    ///////////////////////////////////////////////////////////////////////////////

    ::winrt::fire_and_forget PageGeneral::AutorunGetFAF(VOID)
    {        
        co_await AutorunGetAsync();
    }

    ///////////////////////////////////////////////////////////////////////////////

    ::winrt::Windows::Foundation::IAsyncAction PageGeneral::AutorunEnableAsync(VOID)
    {
        winrt::apartment_context ui_thread; // Capture calling context.
        prgAutorun().IsActive(true);

        co_await winrt::resume_background();
        // Do compute-bound work here.

        ::winrt::Windows::ApplicationModel::StartupTask startupTask =
            co_await ::winrt::Windows::ApplicationModel::StartupTask::GetAsync(L"Wakey");

        ::winrt::Windows::ApplicationModel::StartupTaskState startupState =
            co_await startupTask.RequestEnableAsync();

        co_await ui_thread; // Switch back to calling context.
        prgAutorun().IsActive(false);

        if (tglAutorun().IsOn())
        {
            if (startupState != ::winrt::Windows::ApplicationModel::StartupTaskState::Enabled)
            {
                fntIcoAutorun().Glyph(GLYPH_AUTORUN_OFF);
                tglAutorun().IsOn(false);

                ::winrt::Windows::System::Launcher::LaunchUriAsync(
                    ::winrt::Windows::Foundation::Uri(L"ms-settings:startupapps")
                );
            }
            else
            {
                fntIcoAutorun().Glyph(GLYPH_AUTORUN_ON);
            }
        }
        else
        {
            fntIcoAutorun().Glyph(GLYPH_AUTORUN_OFF);
            startupTask.Disable();
        }

        prgAutorun().IsActive(false);
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageGeneral::tglRunAsAdminAlwaysOnToggled(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (!m_bLoading)
        {
            if (Settings::Get(Settings::SideLoaded, false))
            {
                if (Program::IsRunAsAdmin())
                {
                    Settings::Set(
                        Settings::RunAsAdminAlways, tglRunAsAdminAlways().IsOn()
                    );
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
}
