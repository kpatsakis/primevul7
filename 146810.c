  double time_millis_since_init() {
    return OS::TimeCurrentMillis() - time_millis_at_init_;
  }