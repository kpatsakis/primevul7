  void SetCurrentVMState(StateTag state) {
    if (RuntimeProfiler::IsEnabled()) {
      // Make sure thread local top is initialized.
      ASSERT(thread_local_top_.isolate_ == this);
      StateTag current_state = thread_local_top_.current_vm_state_;
      if (current_state != JS && state == JS) {
        // Non-JS -> JS transition.
        RuntimeProfiler::IsolateEnteredJS(this);
      } else if (current_state == JS && state != JS) {
        // JS -> non-JS transition.
        ASSERT(RuntimeProfiler::IsSomeIsolateInJS());
        RuntimeProfiler::IsolateExitedJS(this);
      } else {
        // Other types of state transitions are not interesting to the
        // runtime profiler, because they don't affect whether we're
        // in JS or not.
        ASSERT((current_state == JS) == (state == JS));
      }
    }
    thread_local_top_.current_vm_state_ = state;
  }