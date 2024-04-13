  bool has_pending_exception() {
    return !thread_local_top_.pending_exception_->IsTheHole();
  }