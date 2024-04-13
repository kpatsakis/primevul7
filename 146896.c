  Logger* logger() {
    // Call InitializeLoggingAndCounters() if logging is needed before
    // the isolate is fully initialized.
    ASSERT(logger_ != NULL);
    return logger_;
  }