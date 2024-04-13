  void set_external_callback(Address callback) {
    thread_local_top_.external_callback_ = callback;
  }