  MaybeObject** pending_exception_address() {
    return &thread_local_top_.pending_exception_;
  }