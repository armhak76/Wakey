#include "pch.h"
#include "BlueTooth.h"
#if __has_include("BlueTooth.g.cpp")
    #include "BlueTooth.g.cpp"
#endif

using namespace winrt;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::wakey::implementation
{
    ///////////////////////////////////////////////////////////////////////////////

    BlueTooth::BlueTooth(VOID)
        : m_devWatcher(nullptr)
        , m_vecRadios (nullptr)
        , m_isEnabled (false)
        , m_bStarted  (false)
    {
        m_mapDevices = winrt::single_threaded_observable_map<
            ::winrt::hstring, ::winrt::Windows::Foundation::IInspectable
        >();
        m_mapConnected = winrt::single_threaded_observable_map<
            ::winrt::hstring, bool
        >();

        EnumRadios();
    }

    ///////////////////////////////////////////////////////////////////////////////

    void BlueTooth::Close(VOID)
    {
        if (m_devWatcher != nullptr)
        {
            m_ehDevices(
                winrt::hstring(),
                BluetoothDeviceChange::EnumStopped
            );

            // Unregister the event handlers.
            m_devWatcher.Added  (m_etDevAdded);
            m_devWatcher.Updated(m_etDevUpdated);
            m_devWatcher.Removed(m_etDevRemoved);
            m_devWatcher.EnumerationCompleted(m_etDevEnumCom);
            m_devWatcher.Stopped(m_etDevStopped);

            // Stop the watcher.
            if(m_bStarted)
                m_devWatcher.Stop();

            m_devWatcher = nullptr;
        }

        if (m_mapDevices.Size())
            m_mapDevices.Clear();
        if (m_mapConnected.Size())
            m_mapConnected.Clear();
    }

    ///////////////////////////////////////////////////////////////////////////////

    bool BlueTooth::IsEnabled(VOID)
    {
        return m_isEnabled;
    }

    ///////////////////////////////////////////////////////////////////////////////

    ::winrt::fire_and_forget BlueTooth::IsEnabled(_In_ bool bEnabled)
    {
        winrt::Windows::Devices::Bluetooth::BluetoothAdapter btAdapter = 
            co_await winrt::Windows::Devices::Bluetooth::BluetoothAdapter::GetDefaultAsync();

        if (btAdapter)
        {
            winrt::Windows::Devices::Radios::Radio radio = 
                co_await btAdapter.GetRadioAsync();

            if (radio)
            {
                if(!m_isEnabled && bEnabled)
                    co_await radio.SetStateAsync(winrt::Windows::Devices::Radios::RadioState::On);
                 if(m_isEnabled && !bEnabled)
                    co_await radio.SetStateAsync(winrt::Windows::Devices::Radios::RadioState::Off);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    ::winrt::fire_and_forget BlueTooth::EnumRadios(VOID)
    {
        winrt::Windows::Devices::Radios::RadioAccessStatus accessLevel =
            co_await Windows::Devices::Radios::Radio::RequestAccessAsync();

        if (accessLevel != winrt::Windows::Devices::Radios::RadioAccessStatus::Allowed)
            co_return;

        m_vecRadios =
            co_await winrt::Windows::Devices::Radios::Radio::GetRadiosAsync();

        for (winrt::Windows::Devices::Radios::Radio radio : m_vecRadios)
        {
            if (radio.Kind() == winrt::Windows::Devices::Radios::RadioKind::Bluetooth)
            {
                if (radio.State() == winrt::Windows::Devices::Radios::RadioState::On)
                {
                    m_isEnabled = true;
                    DevicesEnumStart();
                }
                else
                {
                    m_isEnabled = false;
                    DevicesEnumStop();
                }

                m_etStatus = radio.StateChanged(
                    { get_weak(), &BlueTooth::State_Changed } // TODO get_weak()
                );
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////

    ::winrt::fire_and_forget BlueTooth::State_Changed(
        _In_ winrt::Windows::Devices::Radios::Radio const& radio,
        _In_ winrt::Windows::Foundation::IInspectable const& args
    )
    {
        UNREFERENCED_PARAMETER(args);

        if (radio.State() == winrt::Windows::Devices::Radios::RadioState::On)
        {
            m_isEnabled = true;
            DevicesEnumStart();
            m_ehState(BluetoothStatus::StatusEnabled);
        }
        else
        {
            m_isEnabled = false;
            DevicesEnumStop();
            m_ehState(BluetoothStatus::StatusDisabled);
        }

        co_return;
    }

    ///////////////////////////////////////////////////////////////////////////////

    ::winrt::event_token BlueTooth::OnDeviceChanged(
        _In_ wakey::OnDeviceChangedHandler const& handler
    )
    {
        return m_ehDevices.add(handler);
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID BlueTooth::OnDeviceChanged(
        _In_ winrt::event_token const& token
    )
    {
        m_ehDevices.remove(token);
    }

    ///////////////////////////////////////////////////////////////////////////////

    ::winrt::event_token BlueTooth::OnStatusChanged(
        _In_ wakey::OnStatusChangedHandler const& handler
    )
    {
        return m_ehState.add(handler);
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID BlueTooth::OnStatusChanged(
        _In_ winrt::event_token const& token
    )
    {
        m_ehState.remove(token);
    }


    ///////////////////////////////////////////////////////////////////////////////
    /*
    VOID BlueTooth::MapChangeEventHandler(
        _In_ winrt::Windows::Foundation::Collections::IObservableMap<::winrt::hstring, ::winrt::Windows::Foundation::IInspectable> const& sender,
        _In_ winrt::Windows::Foundation::Collections::IMapChangedEventArgs<::winrt::hstring> const& event
    )
    {
        int i = 0;
    }
    */
    ///////////////////////////////////////////////////////////////////////////////

    VOID BlueTooth::DevicesEnumStart(VOID)
    {
        if (m_devWatcher == nullptr)
        {
            // Additional properties we would like about the device.
            // Property strings are documented here https://msdn.microsoft.com/en-us/library/windows/desktop/ff521659(v=vs.85).aspx

            winrt::Windows::Foundation::Collections::IVector<winrt::hstring> vecRequestedProperties =
                winrt::single_threaded_vector<winrt::hstring>(
                    {
                        L"System.Devices.Aep.DeviceAddress",
                        L"System.Devices.Aep.IsPaired",
                        L"System.Devices.Aep.IsConnected",
                        L"System.Devices.Aep.Category"
                    }
                );

            // BT_Code: Example showing paired and non-paired in a single query.
            winrt::hstring htsrAllBleDevices =
                L"(System.Devices.Aep.ProtocolId:=\"{e0cbf06c-cd8b-4647-bb8a-263b43f0f974}\")"; // { bb7bb05e - 5972 - 42b5 - 94fc - 76eaa7084d49 }\")";

            m_devWatcher =
                winrt::Windows::Devices::Enumeration::DeviceInformation::CreateWatcher(
                    htsrAllBleDevices,
                    vecRequestedProperties,
                    winrt::Windows::Devices::Enumeration::DeviceInformationKind::AssociationEndpoint
                );

            // Register event handlers before starting the watcher.
            m_etDevAdded   = m_devWatcher.Added(
                { get_weak(), &BlueTooth::Device_Added}
            );
            m_etDevUpdated = m_devWatcher.Updated(
                { get_weak(), &BlueTooth::Device_Updated }
            );
            m_etDevRemoved = m_devWatcher.Removed(
                { get_weak(), &BlueTooth::Device_Removed }
            );
            m_etDevEnumCom = m_devWatcher.EnumerationCompleted(
                { get_weak(), &BlueTooth::Device_EnumCom }
            );
            m_etDevStopped = m_devWatcher.Stopped(
                { get_weak(), &BlueTooth::Device_Stopped }
            );

            // Start over with an empty collection.
            //m_knownDevices.Clear();
        }

        // Start the watcher. Active enumeration is limited to approximately 30 seconds.
        // This limits power usage and reduces interference with other BlueTooth activities.
        // To monitor for the presence of BlueTooth LE devices for an extended period,
        // use the BluetoothLEAdvertisementWatcher runtime class. See the BluetoothAdvertisement
        // sample for an example.

        m_bStarted = true;
        m_devWatcher.Start();
        m_ehDevices(
            winrt::hstring(),
            BluetoothDeviceChange::EnumStarted
        );
    }

    ///////////////////////////////////////////////////////////////////////////////

    VOID BlueTooth::DevicesEnumStop(VOID)
    {
        if (m_devWatcher != nullptr)
        {
            m_ehDevices(
                winrt::hstring(),
                BluetoothDeviceChange::EnumStopped
            );

            m_bStarted = false;
            m_devWatcher.Stop();
        }

        if (m_mapDevices.Size())
            m_mapDevices.Clear();
        if (m_mapConnected.Size())
            m_mapConnected.Clear();
    }

    ///////////////////////////////////////////////////////////////////////////////

    ::winrt::Windows::Foundation::Collections::IObservableMap<
        winrt::hstring, Windows::Foundation::IInspectable
    > BlueTooth::GetDevices(VOID)
    {
        return m_mapDevices;
    }

    ///////////////////////////////////////////////////////////////////////////////

    ::winrt::Windows::Foundation::Collections::IObservableMap<
        winrt::hstring, bool
    > BlueTooth::GetConnections(VOID)
    {
        return m_mapConnected;
    }

    ///////////////////////////////////////////////////////////////////////////////

    bool BlueTooth::LookupIsDeviceConnectedById(
        _In_ winrt::hstring const& id
    )
    {
        ::winrt::Windows::Devices::Enumeration::DeviceInformation di =
            m_mapDevices.Lookup(id).try_as<
                ::winrt::Windows::Devices::Enumeration::DeviceInformation
            >();

        if (di != nullptr)
            return LookupIsDeviceConnected(di);

        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////

    bool BlueTooth::LookupIsDeviceConnected(
        _In_::winrt::Windows::Devices::Enumeration::DeviceInformation const& di
    )
    {
        if (di != nullptr)
        {
            winrt::Windows::Foundation::IInspectable iiProp =
                di.Properties().TryLookup(L"System.Devices.Aep.IsConnected");

            if (iiProp)
                return winrt::unbox_value<bool>(iiProp);
        }

        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////

    bool BlueTooth::LookupIsDeviceConnectedByName(
        _In_ winrt::hstring const& name
    )
    {
        ::winrt::Windows::Devices::Enumeration::DeviceInformation di =
            LookupDeviceByName(name);

        if (di != nullptr)
            return LookupIsDeviceConnected(di);

        return false;
    }
    
    ///////////////////////////////////////////////////////////////////////////////

    bool BlueTooth::LookupWasDeviceConnectedByName(
        _In_ winrt::hstring const& name
    )
    {
        ::winrt::Windows::Devices::Enumeration::DeviceInformation di =
            LookupDeviceByName(name);

        if (di != nullptr)
            return LookupWasDeviceConnectedById(di.Id());

        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////

    bool BlueTooth::LookupWasDeviceConnectedById(
        _In_ winrt::hstring const& id
    )
    {
        if(m_mapConnected.HasKey(id))
            return m_mapConnected.Lookup(id);

        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::hstring BlueTooth::LookupDeviceNameById(
        _In_ winrt::hstring const& id
    )
    {
        ::winrt::Windows::Devices::Enumeration::DeviceInformation di =
            m_mapDevices.Lookup(id).try_as<
                ::winrt::Windows::Devices::Enumeration::DeviceInformation
            >();

        if (di != nullptr)
            return di.Name();

        return  winrt::hstring();
    }

    ///////////////////////////////////////////////////////////////////////////////

    ::winrt::Windows::Devices::Enumeration::DeviceInformation BlueTooth::LookupDeviceByName(
        _In_ winrt::hstring const& name
    )
    {
        for (winrt::Windows::Foundation::Collections::IKeyValuePair<
                winrt::hstring, Windows::Foundation::IInspectable
        > it : m_mapDevices)
        {
            ::winrt::Windows::Devices::Enumeration::DeviceInformation di = 
                it.Value().try_as<::winrt::Windows::Devices::Enumeration::DeviceInformation>();

            if(di && di.Name() == name)
                return di;
        }

        return nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::fire_and_forget BlueTooth::Device_Added(
        _In_ ::winrt::Windows::Devices::Enumeration::DeviceWatcher const& sender,
        _In_ ::winrt::Windows::Devices::Enumeration::DeviceInformation const& deviceInfo
    )
    {
        //winrt::apartment_context ui_thread; // Capture calling context.

        // We must update the collection on the UI thread 

        //co_await ui_thread;

        if (sender == m_devWatcher)
        {
            // Make sure device isn't already present in the list.
            if (!deviceInfo.Name().empty())
            {
                winrt::hstring hstrId = deviceInfo.Id();

                if(!m_mapDevices.HasKey(hstrId)     && 
                    deviceInfo.Pairing().IsPaired())
                {
                    m_mapConnected.Insert(
                        hstrId, LookupIsDeviceConnected(deviceInfo)
                    );
                    m_mapDevices.Insert(hstrId, deviceInfo);
                    m_ehDevices(
                        hstrId,
                        BluetoothDeviceChange::DeviceAdded
                    );
                }
            }
        }

        co_return;
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::fire_and_forget BlueTooth::Device_Updated(
        _In_ ::winrt::Windows::Devices::Enumeration::DeviceWatcher const& sender,
        _In_ ::winrt::Windows::Devices::Enumeration::DeviceInformationUpdate const& deviceInfoUpdate
    )
    {
        //winrt::apartment_context ui_thread; // Capture calling context.

        // We must update the collection on the UI thread 

        //co_await ui_thread;

        if (sender == m_devWatcher)
        {
            // Make sure device isn't already present in the list.
            if (deviceInfoUpdate != nullptr)
            {
                winrt::hstring hstrId = deviceInfoUpdate.Id();
                if (m_mapDevices.HasKey(hstrId))
                {
                    ::winrt::Windows::Devices::Enumeration::DeviceInformation di =
                        m_mapDevices.Lookup(hstrId).try_as<
                            ::winrt::Windows::Devices::Enumeration::DeviceInformation
                        >();

                    if (di != nullptr && 
                        di.Pairing().IsPaired())
                    {
                        di.Update(deviceInfoUpdate);

                        for (winrt::Windows::Foundation::Collections::IKeyValuePair<winrt::hstring, bool> it : m_mapConnected)
                        {
                            if (it.Key() == hstrId)
                            {
                                bool bConnected = LookupIsDeviceConnectedById(hstrId);
                                if (bConnected)
                                {
                                    m_mapConnected.Remove(hstrId);
                                    m_mapConnected.Insert(hstrId, bConnected);
                                    break;
                                }
                            }
                        }
                        
                        m_ehDevices(
                            hstrId,
                            BluetoothDeviceChange::DeviceUpdated
                        );
                    }
                }
            }
        }

        co_return;
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::fire_and_forget BlueTooth::Device_Removed(
        _In_ ::winrt::Windows::Devices::Enumeration::DeviceWatcher const& sender,
        _In_ ::winrt::Windows::Devices::Enumeration::DeviceInformationUpdate const& deviceInfoUpdate
    )
    {
       //winrt::apartment_context ui_thread; // Capture calling context.

        // We must update the collection on the UI thread 

        //co_await ui_thread;

        if (sender == m_devWatcher)
        {
            // Make sure device isn't already present in the list.
            if (deviceInfoUpdate != nullptr)
            {
                winrt::hstring hstrId = deviceInfoUpdate.Id();
                if (m_mapDevices.HasKey(hstrId))
                {
                    m_ehDevices(
                        hstrId,
                        BluetoothDeviceChange::DeviceRemoved
                    );
                    m_mapDevices.Remove(hstrId);
                    m_mapConnected.Remove(hstrId);
                }
            }
        }

        co_return;
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::fire_and_forget BlueTooth::Device_EnumCom(
        _In_ ::winrt::Windows::Devices::Enumeration::DeviceWatcher const& sender,
        _In_ ::winrt::Windows::Foundation::IInspectable const& ii
    )
    {
        //winrt::apartment_context ui_thread; // Capture calling context.

        // We must update the collection on the UI thread 

        //co_await ui_thread;

        if (sender == m_devWatcher)
        {
            m_ehDevices(
                winrt::hstring(),
                BluetoothDeviceChange::EnumComplete
            );
        }

        co_return;
    }

    ///////////////////////////////////////////////////////////////////////////////

    winrt::fire_and_forget BlueTooth::Device_Stopped(
        _In_::winrt::Windows::Devices::Enumeration::DeviceWatcher const& sender,
        _In_::winrt::Windows::Foundation::IInspectable const& ii)
    {
        //winrt::apartment_context ui_thread; // Capture calling context.

        // We must update the collection on the UI thread 

        //co_await ui_thread;

        if (sender == m_devWatcher)
        {
            m_ehDevices(
                winrt::hstring(),
                BluetoothDeviceChange::EnumStopped
            );
        }

        co_return;
    }
    ///////////////////////////////////////////////////////////////////////////////
}
