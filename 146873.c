  ~AssertNoContextChange() {
    ASSERT(Isolate::Current()->context() == *context_);
  }