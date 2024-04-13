  ~PostponeInterruptsScope() {
    if (--stack_guard_->thread_local_.postpone_interrupts_nesting_ == 0) {
      stack_guard_->EnableInterrupts();
    }
  }