  Debug* debug() {
    if (!NoBarrier_Load(&debugger_initialized_)) InitializeDebugger();
    return debug_;
  }