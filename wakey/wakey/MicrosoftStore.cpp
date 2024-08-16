#include "pch.h"
#include "MicrosoftStore.h"

#if __has_include("MicrosoftStore.g.cpp")
    #include "MicrosoftStore.g.cpp"
#endif

#include <Shobjidl.h>
using namespace winrt;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::wakey::implementation
{
    ///////////////////////////////////////////////////////////////////////////////

    LPCWSTR MicrosoftStore::s_pwszStoreID  = L"9NQVJ8DCS742";
    LPCWSTR MicrosoftStore::s_pwszProdID   = L"9NBLGGH4TNMN";
    LPCWSTR MicrosoftStore::s_pwszProdKind = L"Consumable";

    ///////////////////////////////////////////////////////////////////////////////

    MicrosoftStore::MicrosoftStore(_In_ winrt::Microsoft::UI::Xaml::Window window)
        : m_storeContext(winrt::Windows::Services::Store::StoreContext::GetDefault())
    {
        WINRT_ASSERT(m_storeContext);
        if (m_storeContext)
        {
            HWND hWnd = Wnd::GetHwnd(window);
            WINRT_ASSERT(hWnd);
            if (hWnd)
            {
                ::winrt::com_ptr<IInitializeWithWindow> iiww =
                    m_storeContext.try_as<IInitializeWithWindow>();

                WINRT_ASSERT(iiww);
                if (iiww)
                    iiww->Initialize(hWnd);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID MicrosoftStore::Close(VOID)
    {
        m_storeContext = nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::Windows::Foundation::IAsyncOperation<
        winrt::hstring
    > MicrosoftStore::PurchaseAsync(VOID)
    {
        WINRT_ASSERT(m_storeContext);

        //winrt::apartment_context ui_thread; // Capture calling context.
        co_await winrt::resume_background();

        
        winrt::hstring hstrMessage = L"resPurchaseUnknownErrMsg";
        if (m_storeContext)
        {
            winrt::Windows::Foundation::Collections::IVector<winrt::hstring> productKinds = 
                winrt::single_threaded_vector<winrt::hstring>();

            //productKinds.Append(L"Durable");
            productKinds.Append(s_pwszProdKind);

            winrt::Windows::Foundation::Collections::IVector<winrt::hstring> storeIDs =
                winrt::single_threaded_vector<winrt::hstring>();

            storeIDs.Append(s_pwszStoreID);

            winrt::Windows::Services::Store::StoreProductQueryResult storeProductQueryResult =
                co_await m_storeContext.GetStoreProductsAsync(productKinds, storeIDs);

            if (storeProductQueryResult.ExtendedError().value == S_OK)
            {
                winrt::Windows::Services::Store::StoreProduct storeProd = nullptr;

                if(storeProductQueryResult.Products().HasKey(s_pwszProdID))
                    storeProd = storeProductQueryResult.Products().Lookup(s_pwszProdID);

                if (storeProd)
                {
                    if (!storeProd.IsInUserCollection())
                    {
                        winrt::Windows::Services::Store::StorePurchaseResult storePurchaseResult =
                            co_await storeProd.RequestPurchaseAsync();

                        switch (storePurchaseResult.Status())
                        {
                        case winrt::Windows::Services::Store::StorePurchaseStatus::AlreadyPurchased:
                            hstrMessage = L"resPurchaseAlreadyMsg";
                            break;
                        case winrt::Windows::Services::Store::StorePurchaseStatus::Succeeded:
                            hstrMessage = L"resPurchaseSuccessMsg";
                            break;
                        case winrt::Windows::Services::Store::StorePurchaseStatus::NotPurchased:
                            hstrMessage = L"resPurchaseCancelledMsg";
                            break;
                        case winrt::Windows::Services::Store::StorePurchaseStatus::NetworkError:
                            hstrMessage = L"resPurchaseNetworkErrMsg";
                            break;
                        case winrt::Windows::Services::Store::StorePurchaseStatus::ServerError:
                            hstrMessage = L"resPurchaseServerErrMsg";
                            break;
                        default:
                            break;
                        }
                    }
                    else
                    {
                        hstrMessage = L"resPurchaseOwnMsg";
                    }
                }                
            }
        }

        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader resLoader;
        co_return resLoader.GetString(hstrMessage);
    }
    ///////////////////////////////////////////////////////////////////////////////

    winrt::Windows::Foundation::IAsyncAction MicrosoftStore::CheckSideLoading(VOID)
    {
        if (!Settings::Get(Settings::SettingType::SideLoaded, false))
        {
            winrt::Windows::Services::Store::StoreAppLicense appLicense =
                co_await m_storeContext.GetAppLicenseAsync();

            if (appLicense.SkuStoreId().empty())
            {
                Settings::Set(
                    Settings::SettingType::SideLoaded, true
                );
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::Windows::Foundation::IAsyncAction MicrosoftStore::CheckPurchase(VOID)
    {
        WINRT_ASSERT(m_storeContext);

        if (Settings::Get(Settings::SettingType::Purchased, false) ||
            Settings::Get(Settings::SettingType::SideLoaded, false))
        {
            co_return;
        }

        //winrt::apartment_context ui_thread; // Capture calling context.
        co_await winrt::resume_background();

        if (m_storeContext)
        {
            winrt::Windows::Foundation::Collections::IVector<winrt::hstring> productKinds =
                winrt::single_threaded_vector<winrt::hstring>();

            //productKinds.Append(L"Durable");
            productKinds.Append(s_pwszProdKind);

            winrt::Windows::Services::Store::StoreProductQueryResult storeProductQueryResult = 
                co_await m_storeContext.GetUserCollectionAsync(productKinds);

            if (storeProductQueryResult.ExtendedError().value == S_OK)
            {
                winrt::Windows::Services::Store::StoreProduct storeProd = nullptr;

                if(storeProductQueryResult.Products().HasKey(s_pwszProdID))
                    storeProd = storeProductQueryResult.Products().Lookup(s_pwszProdID);

                if (storeProd)
                {
                    Settings::Set(
                        Settings::SettingType::Purchased, true
                    );
                }
            }
        }

        //co_await ui_thread;
    }


    ///////////////////////////////////////////////////////////////////////////////
    
    winrt::Windows::Foundation::IAsyncOperation<
        winrt::hstring
    > MicrosoftStore::UpdateAsync(VOID)
    {
        WINRT_ASSERT(m_storeContext);

        //winrt::apartment_context ui_thread; // Capture calling context.
        co_await winrt::resume_background(); 
        
        winrt::hstring hstrMessage = L"resUpdateFailedMsg";
        if (m_storeContext)
        {
            winrt::Windows::Services::Store::StoreProductResult spr =
                co_await m_storeContext.GetStoreProductForCurrentAppAsync();

            if (spr.ExtendedError().value == S_OK)
            {
                if (m_storeContext.CanSilentlyDownloadStorePackageUpdates())
                {
                    winrt::Windows::Foundation::Collections::IVectorView<
                        winrt::Windows::Services::Store::StorePackageUpdate
                    > updates =
                        co_await m_storeContext.GetAppAndOptionalStorePackageUpdatesAsync();

                    winrt::Windows::Services::Store::StorePackageUpdateResult result =
                        co_await m_storeContext.TrySilentDownloadAndInstallStorePackageUpdatesAsync(updates);

                    switch (result.OverallState())
                    {
                    case winrt::Windows::Services::Store::StorePackageUpdateState::Pending:
                        hstrMessage = L"resUpdatePendingMsg";
                        break;
                    case winrt::Windows::Services::Store::StorePackageUpdateState::Downloading:
                        hstrMessage = L"resUpdateDownloadingMsg";
                        break;
                    case winrt::Windows::Services::Store::StorePackageUpdateState::Deploying:
                        hstrMessage = L"resUpdateDeployedMsg";
                        break;
                    case winrt::Windows::Services::Store::StorePackageUpdateState::Completed:
                        hstrMessage = L"resUpdateCompletedMsg";
                        break;
                    case winrt::Windows::Services::Store::StorePackageUpdateState::Canceled:
                        hstrMessage = L"resUpdateCancelledMsg";
                        break;
                    case winrt::Windows::Services::Store::StorePackageUpdateState::OtherError:
                        hstrMessage = L"resUpdateUnknownErrMsg";
                        break;
                    case winrt::Windows::Services::Store::StorePackageUpdateState::ErrorLowBattery:
                        hstrMessage = L"resUpdateLowBatteryMsg";
                        break;
                    case winrt::Windows::Services::Store::StorePackageUpdateState::ErrorWiFiRecommended:
                        hstrMessage = L"resUpdateWiFiRecommendedMsg";
                        break;
                    case winrt::Windows::Services::Store::StorePackageUpdateState::ErrorWiFiRequired:
                        hstrMessage = L"resUpdateWiFiRequiredMsg";
                        break;
                    }
                }
            }
        }

        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader resLoader;
        co_return resLoader.GetString(hstrMessage);
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::Windows::Foundation::IAsyncOperation<bool>
        MicrosoftStore::RateAsync(VOID)
    {
        WINRT_ASSERT(m_storeContext);

        //winrt::apartment_context ui_thread; // Capture calling context.
        co_await winrt::resume_background();

        if (m_storeContext)
        {
            winrt::Windows::Services::Store::StoreProductResult spr =
                co_await m_storeContext.GetStoreProductForCurrentAppAsync();

            if (spr.ExtendedError().value == S_OK)
            {
                co_await m_storeContext.RequestRateAndReviewAppAsync();
                co_return true;
            }
        }
        co_return false;
    }

    ///////////////////////////////////////////////////////////////////////////////
}
