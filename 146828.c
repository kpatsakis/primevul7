  bool is_catchable_by_javascript(MaybeObject* exception) {
    return (exception != Failure::OutOfMemoryException()) &&
        (exception != heap()->termination_exception());
  }