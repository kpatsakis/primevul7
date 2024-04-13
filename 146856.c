  INLINE(static Isolate* Current()) {
    Isolate* isolate = reinterpret_cast<Isolate*>(
        Thread::GetExistingThreadLocal(isolate_key_));
    ASSERT(isolate != NULL);
    return isolate;
  }