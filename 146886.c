  static PerIsolateThreadData* CurrentPerIsolateThreadData() {
    return reinterpret_cast<PerIsolateThreadData*>(
        Thread::GetThreadLocal(per_isolate_thread_data_key_));
  }