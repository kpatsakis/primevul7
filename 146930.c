  static bool TryLock(Isolate* isolate) {
    return isolate->break_access_->TryLock();
  }