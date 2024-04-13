  void set_catcher(v8::TryCatch* catcher) {
    thread_local_top_.catcher_ = catcher;
  }