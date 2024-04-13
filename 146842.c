  Debugger* debugger() {
    if (!NoBarrier_Load(&debugger_initialized_)) InitializeDebugger();
    return debugger_;
  }