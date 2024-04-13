  AssertNoContextChange() :
      scope_(Isolate::Current()),
      context_(Isolate::Current()->context(), Isolate::Current()) {
  }