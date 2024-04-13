void LIRGenerator::volatile_field_load(LIR_Address* address, LIR_Opr result,
                                       CodeEmitInfo* info) {
  if (address->type() == T_LONG) {
    address = new LIR_Address(address->base(),
                              address->index(), address->scale(),
                              address->disp(), T_DOUBLE);
    // Transfer the value atomically by using FP moves.  This means
    // the value has to be moved between CPU and FPU registers.  In
    // SSE0 and SSE1 mode it has to be moved through spill slot but in
    // SSE2+ mode it can be moved directly.
    LIR_Opr temp_double = new_register(T_DOUBLE);
    __ volatile_move(LIR_OprFact::address(address), temp_double, T_LONG, info);
    __ volatile_move(temp_double, result, T_LONG);
    if (UseSSE < 2) {
      // no spill slot needed in SSE2 mode because xmm->cpu register move is possible
      set_vreg_flag(result, must_start_in_memory);
    }
  } else {
    __ load(address, result, info);
  }
}