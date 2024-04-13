  MaybeObject** scheduled_exception_address() {
    return &thread_local_top_.scheduled_exception_;
  }