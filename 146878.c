  bool* external_caught_exception_address() {
    return &thread_local_top_.external_caught_exception_;
  }