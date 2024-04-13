  ~NetworkLibraryImpl() {
    network_manager_observers_.Clear();
    if (network_manager_monitor_)
      DisconnectPropertyChangeMonitor(network_manager_monitor_);
    data_plan_observers_.Clear();
    if (data_plan_monitor_)
      DisconnectDataPlanUpdateMonitor(data_plan_monitor_);
    STLDeleteValues(&network_observers_);
    ClearNetworks();
  }
