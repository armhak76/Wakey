#include "pch.h"
#include "PageAbout.xaml.h"
#if __has_include("PageAbout.g.cpp")
    #include "PageAbout.g.cpp"
#endif

using namespace winrt;

#include <Shobjidl.h>
#include "App.xaml.h"
#include "CustomDialog.xaml.h"

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::wakey::implementation
{
    ///////////////////////////////////////////////////////////////////////////////

    PageAbout::PageAbout(VOID)
    {
        // Xaml objects should not call InitializeComponent during construction.
        // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent

        NavigationCacheMode(
            winrt::Microsoft::UI::Xaml::Navigation::NavigationCacheMode::Enabled
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageAbout::Loaded(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);
        
        winrt::Windows::ApplicationModel::Package curPackage =
            ::winrt::Windows::ApplicationModel::Package::Current();

        winrt::Windows::ApplicationModel::PackageId packageId = 
            curPackage.Id();

        winrt::Windows::ApplicationModel::PackageVersion version =
            packageId.Version();
        
        WCHAR wszAppDesc[256] = { 0 };
        StringCchPrintfW(
            wszAppDesc,
            __crt_countof(wszAppDesc),
            L"%s version %d.%d.%d.%d by %s",
            curPackage.DisplayName().c_str(),
            version.Major,
            version.Minor,
            version.Build,
            version.Revision,
            curPackage.PublisherDisplayName().c_str()
        );
        txtAppDesc().Text(wszAppDesc);

        if(Settings::Get(Settings::SettingType::Purchased, false))
            ascPro().IsEnabled(false);


        //prgPurchase().IsActive(s_bPurchaseInProgress);
    } 

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageAbout::OnNavigatedTo(
        _In_ winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(args);
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID PageAbout::OnNavigatedFrom(
        _In_ winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(args);
    }

    ///////////////////////////////////////////////////////////////////////////////
    
    winrt::fire_and_forget PageAbout::btnUpdateClick(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);
        
        if (Settings::Get(Settings::SideLoaded, false))
        {
            co_await ShowStoreDialog(__FUNCTIONW__);
            co_return;
        }

        //winrt::apartment_context ui_thread;

        MainWindow* pMainWnd = MainWindow::Get();
        if (pMainWnd->m_msStore)
        {
            winrt::weak_ref<PageAbout> pageAbout = get_weak();
            prgUpdate().IsActive(true);
            btnUpdate().IsEnabled(false);

            winrt::hstring hstrMessage = 
                co_await pMainWnd->m_msStore->UpdateAsync();
                        
            if (pageAbout.get())
            {
                pageAbout.get()->prgUpdate().IsActive(false);
                pageAbout.get()->btnUpdate().IsEnabled(true);
            }

            co_await ShowSimpleDialog(hstrMessage, __FUNCTIONW__);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::fire_and_forget PageAbout::btnRateClick(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (Settings::Get(Settings::SideLoaded, false))
        {
            co_await ShowStoreDialog(__FUNCTIONW__);
            co_return;
        }

        MainWindow* pMainWnd = MainWindow::Get();
        if (pMainWnd->m_msStore)
        {
            winrt::weak_ref<PageAbout> pageAbout = get_weak();
            prgRate().IsActive(true);
            btnRate().IsEnabled(false);

            bool bResult = 
                co_await pMainWnd->m_msStore->RateAsync();

            if (pageAbout.get())
            {
                pageAbout.get()->prgRate().IsActive(false);
                pageAbout.get()->btnRate().IsEnabled(true);
            }

            if (!bResult)
            {
                winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader resLoader;
                co_await ShowSimpleDialog(resLoader.GetString(L"resRateFailedMsg"), __FUNCTIONW__);
             }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::fire_and_forget PageAbout::btnProClick(
        _In_::winrt::Windows::Foundation::IInspectable const& sender,
        _In_::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
    )
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(args);

        if (Settings::Get(Settings::SideLoaded, false))
        {
            co_await ShowStoreDialog(__FUNCTIONW__);
            co_return;
        }

        MainWindow* pMainWnd = MainWindow::Get();
        if (pMainWnd->m_msStore)
        {
            winrt::weak_ref<PageAbout> pageAbout = get_weak();
            prgPurchase().IsActive(true);
            btnPro().IsEnabled(false);

            winrt::hstring hstrMessage =
                co_await pMainWnd->m_msStore->PurchaseAsync();

            co_await pMainWnd->m_msStore->CheckPurchase();
            if (pageAbout.get())
            {
                pageAbout.get()->prgPurchase().IsActive(false);
                pageAbout.get()->btnPro().IsEnabled(true);

                if (Settings::Get(Settings::SettingType::Purchased, false))
                    pageAbout.get()->btnPro().IsEnabled(false);
            }

            co_await ShowSimpleDialog(hstrMessage, __FUNCTIONW__);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::Windows::Foundation::IAsyncAction
        PageAbout::ShowStoreDialog(
            _In_ winrt::hstring hstrFunction
        )
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader resLoader;
        co_await ShowSimpleDialog(resLoader.GetString(L"resPurchaseFromMsStoreMsg"), hstrFunction);
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::Windows::Foundation::IAsyncAction
        PageAbout::ShowSimpleDialog(
            _In_ winrt::hstring hstrMessage,
            _In_ winrt::hstring hstrFunction
        )
    {
        ::winrt::Microsoft::UI::Xaml::Controls::ContentDialog customDialog;
        customDialog = winrt::make<
            winrt::wakey::implementation::CustomDialog>(
                winrt::wakey::IconType::Info, hstrMessage
            );

        customDialog.XamlRoot(
            MainWindow::Get()->Content().XamlRoot()
        );

        customDialog.CloseButtonText(L"OK");
        co_await MainWindow::Get()->ShowDialog(
            customDialog, hstrFunction
        );
    }

    ///////////////////////////////////////////////////////////////////////////////
}
