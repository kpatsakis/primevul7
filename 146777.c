  INLINE(static Isolate* UncheckedCurrent()) {
    return reinterpret_cast<Isolate*>(Thread::GetThreadLocal(isolate_key_));
  }