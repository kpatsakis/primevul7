  void set_pending_exception(MaybeObject* exception) {
    thread_local_top_.pending_exception_ = exception;
  }