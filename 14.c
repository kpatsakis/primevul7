CellularNetwork::CellularNetwork()
    : WirelessNetwork(),
      activation_state_(ACTIVATION_STATE_UNKNOWN),
      network_technology_(NETWORK_TECHNOLOGY_UNKNOWN),
      roaming_state_(ROAMING_STATE_UNKNOWN),
      restricted_pool_(false),
      prl_version_(0) {
  type_ = TYPE_CELLULAR;
}
