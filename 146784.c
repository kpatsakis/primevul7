  void clear_pending_exception() {
    thread_local_top_.pending_exception_ = heap_.the_hole_value();
  }