void LIRGenerator::do_ArithmeticOp_FPU(ArithmeticOp* x) {
  LIRItem left(x->x(),  this);
  LIRItem right(x->y(), this);
  LIRItem* left_arg  = &left;
  LIRItem* right_arg = &right;
  assert(!left.is_stack() || !right.is_stack(), "can't both be memory operands");
  bool must_load_both = (x->op() == Bytecodes::_frem || x->op() == Bytecodes::_drem);
  if (left.is_register() || x->x()->type()->is_constant() || must_load_both) {
    left.load_item();
  } else {
    left.dont_load_item();
  }

  // do not load right operand if it is a constant.  only 0 and 1 are
  // loaded because there are special instructions for loading them
  // without memory access (not needed for SSE2 instructions)
  bool must_load_right = false;
  if (right.is_constant()) {
    LIR_Const* c = right.result()->as_constant_ptr();
    assert(c != NULL, "invalid constant");
    assert(c->type() == T_FLOAT || c->type() == T_DOUBLE, "invalid type");

    if (c->type() == T_FLOAT) {
      must_load_right = UseSSE < 1 && (c->is_one_float() || c->is_zero_float());
    } else {
      must_load_right = UseSSE < 2 && (c->is_one_double() || c->is_zero_double());
    }
  }

  if (must_load_both) {
    // frem and drem destroy also right operand, so move it to a new register
    right.set_destroys_register();
    right.load_item();
  } else if (right.is_register() || must_load_right) {
    right.load_item();
  } else {
    right.dont_load_item();
  }
  LIR_Opr reg = rlock(x);
  LIR_Opr tmp = LIR_OprFact::illegalOpr;
  if (x->is_strictfp() && (x->op() == Bytecodes::_dmul || x->op() == Bytecodes::_ddiv)) {
    tmp = new_register(T_DOUBLE);
  }

  if ((UseSSE >= 1 && x->op() == Bytecodes::_frem) || (UseSSE >= 2 && x->op() == Bytecodes::_drem)) {
    // special handling for frem and drem: no SSE instruction, so must use FPU with temporary fpu stack slots
    LIR_Opr fpu0, fpu1;
    if (x->op() == Bytecodes::_frem) {
      fpu0 = LIR_OprFact::single_fpu(0);
      fpu1 = LIR_OprFact::single_fpu(1);
    } else {
      fpu0 = LIR_OprFact::double_fpu(0);
      fpu1 = LIR_OprFact::double_fpu(1);
    }
    __ move(right.result(), fpu1); // order of left and right operand is important!
    __ move(left.result(), fpu0);
    __ rem (fpu0, fpu1, fpu0);
    __ move(fpu0, reg);

  } else {
    arithmetic_op_fpu(x->op(), reg, left.result(), right.result(), x->is_strictfp(), tmp);
  }

  set_result(x, round_item(reg));
}