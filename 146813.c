  explicit PostponeInterruptsScope(Isolate* isolate)
      : stack_guard_(isolate->stack_guard()) {
    stack_guard_->thread_local_.postpone_interrupts_nesting_++;
    stack_guard_->DisableInterrupts();
  }