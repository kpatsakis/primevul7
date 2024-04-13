  inline bool HasOverflowed() const {
    StackGuard* stack_guard = isolate_->stack_guard();
    return reinterpret_cast<uintptr_t>(this) < stack_guard->real_climit();
  }