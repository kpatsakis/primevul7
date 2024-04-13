  bool has_scheduled_exception() {
    return thread_local_top_.scheduled_exception_ != heap_.the_hole_value();
  }