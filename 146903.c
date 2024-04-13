  Address pending_message_obj_address() {
    return reinterpret_cast<Address>(&thread_local_top_.pending_message_obj_);
  }