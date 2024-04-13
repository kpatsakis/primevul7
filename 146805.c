  Address has_pending_message_address() {
    return reinterpret_cast<Address>(&thread_local_top_.has_pending_message_);
  }