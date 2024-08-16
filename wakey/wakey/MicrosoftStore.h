#pragma once

#include "MicrosoftStore.g.h"

namespace winrt::wakey::implementation
{
    struct MicrosoftStore : MicrosoftStoreT<MicrosoftStore>
    {
        static LPCWSTR s_pwszStoreID;
        static LPCWSTR s_pwszProdID;
        static LPCWSTR s_pwszProdKind;

        ::winrt::Windows::Services::Store::StoreContext m_storeContext;

        MicrosoftStore(_In_ winrt::Microsoft::UI::Xaml::Window window);
        VOID Close(VOID);

        winrt::Windows::Foundation::IAsyncOperation<winrt::hstring> PurchaseAsync   (VOID);
        winrt::Windows::Foundation::IAsyncOperation<winrt::hstring> UpdateAsync     (VOID);
        winrt::Windows::Foundation::IAsyncOperation<bool>           RateAsync       (VOID);
        winrt::Windows::Foundation::IAsyncAction                    CheckPurchase   (VOID);
        winrt::Windows::Foundation::IAsyncAction                    CheckSideLoading(VOID);
    };
}

namespace winrt::wakey::factory_implementation
{
    struct MicrosoftStore : MicrosoftStoreT<MicrosoftStore, implementation::MicrosoftStore>
    {
    };
}
