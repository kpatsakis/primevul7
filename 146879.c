  void set_save_context(SaveContext* save) {
    thread_local_top_.save_context_ = save;
  }