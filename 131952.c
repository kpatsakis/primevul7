void LIRGenerator::do_ArithmeticOp_Int(ArithmeticOp* x) {
  if (x->op() == Bytecodes::_idiv || x->op() == Bytecodes::_irem) {
    // The requirements for division and modulo
    // input : rax,: dividend                         min_int
    //         reg: divisor   (may not be rax,/rdx)   -1
    //
    // output: rax,: quotient  (= rax, idiv reg)       min_int
    //         rdx: remainder (= rax, irem reg)       0

    // rax, and rdx will be destroyed

    // Note: does this invalidate the spec ???
    LIRItem right(x->y(), this);
    LIRItem left(x->x() , this);   // visit left second, so that the is_register test is valid

    // call state_for before load_item_force because state_for may
    // force the evaluation of other instructions that are needed for
    // correct debug info.  Otherwise the live range of the fix
    // register might be too long.
    CodeEmitInfo* info = state_for(x);

    left.load_item_force(divInOpr());

    right.load_item();

    LIR_Opr result = rlock_result(x);
    LIR_Opr result_reg;
    if (x->op() == Bytecodes::_idiv) {
      result_reg = divOutOpr();
    } else {
      result_reg = remOutOpr();
    }

    if (!ImplicitDiv0Checks) {
      __ cmp(lir_cond_equal, right.result(), LIR_OprFact::intConst(0));
      __ branch(lir_cond_equal, T_INT, new DivByZeroStub(info));
      // Idiv/irem cannot trap (passing info would generate an assertion).
      info = NULL;
    }
    LIR_Opr tmp = FrameMap::rdx_opr; // idiv and irem use rdx in their implementation
    if (x->op() == Bytecodes::_irem) {
      __ irem(left.result(), right.result(), result_reg, tmp, info);
    } else if (x->op() == Bytecodes::_idiv) {
      __ idiv(left.result(), right.result(), result_reg, tmp, info);
    } else {
      ShouldNotReachHere();
    }

    __ move(result_reg, result);
  } else {
    // missing test if instr is commutative and if we should swap
    LIRItem left(x->x(),  this);
    LIRItem right(x->y(), this);
    LIRItem* left_arg = &left;
    LIRItem* right_arg = &right;
    if (x->is_commutative() && left.is_stack() && right.is_register()) {
      // swap them if left is real stack (or cached) and right is real register(not cached)
      left_arg = &right;
      right_arg = &left;
    }

    left_arg->load_item();

    // do not need to load right, as we can handle stack and constants
    if (x->op() == Bytecodes::_imul ) {
      // check if we can use shift instead
      bool use_constant = false;
      bool use_tmp = false;
      if (right_arg->is_constant()) {
        jint iconst = right_arg->get_jint_constant();
        if (iconst > 0 && iconst < max_jint) {
          if (is_power_of_2(iconst)) {
            use_constant = true;
          } else if (is_power_of_2(iconst - 1) || is_power_of_2(iconst + 1)) {
            use_constant = true;
            use_tmp = true;
          }
        }
      }
      if (use_constant) {
        right_arg->dont_load_item();
      } else {
        right_arg->load_item();
      }
      LIR_Opr tmp = LIR_OprFact::illegalOpr;
      if (use_tmp) {
        tmp = new_register(T_INT);
      }
      rlock_result(x);

      arithmetic_op_int(x->op(), x->operand(), left_arg->result(), right_arg->result(), tmp);
    } else {
      right_arg->dont_load_item();
      rlock_result(x);
      LIR_Opr tmp = LIR_OprFact::illegalOpr;
      arithmetic_op_int(x->op(), x->operand(), left_arg->result(), right_arg->result(), tmp);
    }
  }
}