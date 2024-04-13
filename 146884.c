  Counters* counters() {
    // Call InitializeLoggingAndCounters() if logging is needed before
    // the isolate is fully initialized.
    ASSERT(counters_ != NULL);
    return counters_;
  }