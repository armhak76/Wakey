#pragma once

#include "BlueTooth.g.h"

namespace winrt::wakey::implementation
{
    struct BlueTooth : BlueToothT<BlueTooth>
    {
    public:
        BlueTooth(VOID);
        VOID Close(VOID);

        winrt::event_token OnDeviceChanged(_In_ wakey::OnDeviceChangedHandler const& handler);
        VOID OnDeviceChanged(_In_ winrt::event_token const& token);

        winrt::event_token OnStatusChanged(_In_ wakey::OnStatusChangedHandler const& handler);
        VOID OnStatusChanged(_In_ winrt::event_token const& token);
       
        ::winrt::Windows::Foundation::Collections::IObservableMap<::winrt::hstring, winrt::Windows::Foundation::IInspectable> GetDevices(VOID);
        ::winrt::Windows::Foundation::Collections::IObservableMap<::winrt::hstring, bool> GetConnections(VOID);

        ::winrt::Windows::Devices::Enumeration::DeviceInformation LookupDeviceByName(_In_ winrt::hstring const& name);

        bool            IsEnabled                     (VOID);
        bool            LookupWasDeviceConnectedByName(_In_ winrt::hstring const& name);
        bool            LookupWasDeviceConnectedById  (_In_ winrt::hstring const& id);
        bool            LookupIsDeviceConnected       (_In_::winrt::Windows::Devices::Enumeration::DeviceInformation const& di);
        bool            LookupIsDeviceConnectedByName (_In_ winrt::hstring const& name);
        bool            LookupIsDeviceConnectedById   (_In_ winrt::hstring const& id);
        winrt::hstring  LookupDeviceNameById          (_In_ winrt::hstring const& id);
        
        ::winrt::fire_and_forget IsEnabled(_In_ bool bEnabled);

    private:
        bool m_isEnabled;
        bool m_bStarted;

        VOID DevicesEnumStart   (VOID);
        VOID DevicesEnumStop    (VOID);

        ::winrt::Windows::Devices::Enumeration::DeviceWatcher m_devWatcher;

        ::winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Devices::Radios::Radio> m_vecRadios;
        ::winrt::Windows::Foundation::Collections::IObservableMap<winrt::hstring, Windows::Foundation::IInspectable> m_mapDevices;
        ::winrt::Windows::Foundation::Collections::IObservableMap<winrt::hstring, bool> m_mapConnected;

        ::winrt::event_token m_etStatus;
        ::winrt::event_token m_etDevAdded;
        ::winrt::event_token m_etDevUpdated;
        ::winrt::event_token m_etDevRemoved;
        ::winrt::event_token m_etDevEnumCom;
        ::winrt::event_token m_etDevStopped;

        ::winrt::fire_and_forget EnumRadios(VOID);

        ::winrt::fire_and_forget State_Changed  (_In_ ::winrt::Windows::Devices::Radios::Radio const& radio, _In_ ::winrt::Windows::Foundation::IInspectable const& args);
        ::winrt::fire_and_forget Device_Added   (_In_ ::winrt::Windows::Devices::Enumeration::DeviceWatcher const& sender, _In_ ::winrt::Windows::Devices::Enumeration::DeviceInformation const& deviceInfo);
        ::winrt::fire_and_forget Device_Updated (_In_ ::winrt::Windows::Devices::Enumeration::DeviceWatcher const& sender, _In_ ::winrt::Windows::Devices::Enumeration::DeviceInformationUpdate const& deviceInfoUpdate);
        ::winrt::fire_and_forget Device_Removed (_In_ ::winrt::Windows::Devices::Enumeration::DeviceWatcher const& sender, _In_ ::winrt::Windows::Devices::Enumeration::DeviceInformationUpdate const& deviceInfoUpdate);
        ::winrt::fire_and_forget Device_EnumCom (_In_ ::winrt::Windows::Devices::Enumeration::DeviceWatcher const& sender, _In_ ::winrt::Windows::Foundation::IInspectable const& ii);
        ::winrt::fire_and_forget Device_Stopped (_In_ ::winrt::Windows::Devices::Enumeration::DeviceWatcher const& sender, _In_ ::winrt::Windows::Foundation::IInspectable const& ii);

        winrt::event<wakey::OnStatusChangedHandler> m_ehState;
        winrt::event<wakey::OnDeviceChangedHandler> m_ehDevices;
    };
}

namespace winrt::wakey::factory_implementation
{
    struct BlueTooth : BlueToothT<BlueTooth, implementation::BlueTooth>
    {
    };
}
