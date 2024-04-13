  v8::TryCatch* try_catch_handler() {
    return thread_local_top_.TryCatchHandler();
  }