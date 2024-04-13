    virtual ~NetworkObserverList() {
      if (network_monitor_)
        DisconnectPropertyChangeMonitor(network_monitor_);
    }
