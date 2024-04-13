void LIRGenerator::do_ArithmeticOp_Long(ArithmeticOp* x) {
  if (x->op() == Bytecodes::_ldiv || x->op() == Bytecodes::_lrem ) {
    // long division is implemented as a direct call into the runtime
    LIRItem left(x->x(), this);
    LIRItem right(x->y(), this);

    // the check for division by zero destroys the right operand
    right.set_destroys_register();

    BasicTypeList signature(2);
    signature.append(T_LONG);
    signature.append(T_LONG);
    CallingConvention* cc = frame_map()->c_calling_convention(&signature);

    // check for division by zero (destroys registers of right operand!)
    CodeEmitInfo* info = state_for(x);

    const LIR_Opr result_reg = result_register_for(x->type());
    left.load_item_force(cc->at(1));
    right.load_item();

    __ move(right.result(), cc->at(0));

    __ cmp(lir_cond_equal, right.result(), LIR_OprFact::longConst(0));
    __ branch(lir_cond_equal, T_LONG, new DivByZeroStub(info));

    address entry = NULL;
    switch (x->op()) {
    case Bytecodes::_lrem:
      entry = CAST_FROM_FN_PTR(address, SharedRuntime::lrem);
      break; // check if dividend is 0 is done elsewhere
    case Bytecodes::_ldiv:
      entry = CAST_FROM_FN_PTR(address, SharedRuntime::ldiv);
      break; // check if dividend is 0 is done elsewhere
    case Bytecodes::_lmul:
      entry = CAST_FROM_FN_PTR(address, SharedRuntime::lmul);
      break;
    default:
      ShouldNotReachHere();
    }

    LIR_Opr result = rlock_result(x);
    __ call_runtime_leaf(entry, getThreadTemp(), result_reg, cc->args());
    __ move(result_reg, result);
  } else if (x->op() == Bytecodes::_lmul) {
    // missing test if instr is commutative and if we should swap
    LIRItem left(x->x(), this);
    LIRItem right(x->y(), this);

    // right register is destroyed by the long mul, so it must be
    // copied to a new register.
    right.set_destroys_register();

    left.load_item();
    right.load_item();

    LIR_Opr reg = FrameMap::long0_opr;
    arithmetic_op_long(x->op(), reg, left.result(), right.result(), NULL);
    LIR_Opr result = rlock_result(x);
    __ move(reg, result);
  } else {
    // missing test if instr is commutative and if we should swap
    LIRItem left(x->x(), this);
    LIRItem right(x->y(), this);

    left.load_item();
    // don't load constants to save register
    right.load_nonconstant();
    rlock_result(x);
    arithmetic_op_long(x->op(), x->operand(), left.result(), right.result(), NULL);
  }
}