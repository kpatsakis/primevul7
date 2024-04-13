void LIRGenerator::do_LibmIntrinsic(Intrinsic* x) {
  LIRItem value(x->argument_at(0), this);
  value.set_destroys_register();

  LIR_Opr calc_result = rlock_result(x);
  LIR_Opr result_reg = result_register_for(x->type());

  CallingConvention* cc = NULL;

  if (x->id() == vmIntrinsics::_dpow) {
    LIRItem value1(x->argument_at(1), this);

    value1.set_destroys_register();

    BasicTypeList signature(2);
    signature.append(T_DOUBLE);
    signature.append(T_DOUBLE);
    cc = frame_map()->c_calling_convention(&signature);
    value.load_item_force(cc->at(0));
    value1.load_item_force(cc->at(1));
  } else {
    BasicTypeList signature(1);
    signature.append(T_DOUBLE);
    cc = frame_map()->c_calling_convention(&signature);
    value.load_item_force(cc->at(0));
  }

#ifndef _LP64
  LIR_Opr tmp = FrameMap::fpu0_double_opr;
  result_reg = tmp;
  switch(x->id()) {
    case vmIntrinsics::_dexp:
      if (StubRoutines::dexp() != NULL) {
        __ call_runtime_leaf(StubRoutines::dexp(), getThreadTemp(), result_reg, cc->args());
      } else {
        __ call_runtime_leaf(CAST_FROM_FN_PTR(address, SharedRuntime::dexp), getThreadTemp(), result_reg, cc->args());
      }
      break;
    case vmIntrinsics::_dlog:
      if (StubRoutines::dlog() != NULL) {
        __ call_runtime_leaf(StubRoutines::dlog(), getThreadTemp(), result_reg, cc->args());
      } else {
        __ call_runtime_leaf(CAST_FROM_FN_PTR(address, SharedRuntime::dlog), getThreadTemp(), result_reg, cc->args());
      }
      break;
    case vmIntrinsics::_dlog10:
      if (StubRoutines::dlog10() != NULL) {
       __ call_runtime_leaf(StubRoutines::dlog10(), getThreadTemp(), result_reg, cc->args());
      } else {
        __ call_runtime_leaf(CAST_FROM_FN_PTR(address, SharedRuntime::dlog10), getThreadTemp(), result_reg, cc->args());
      }
      break;
    case vmIntrinsics::_dpow:
      if (StubRoutines::dpow() != NULL) {
        __ call_runtime_leaf(StubRoutines::dpow(), getThreadTemp(), result_reg, cc->args());
      } else {
        __ call_runtime_leaf(CAST_FROM_FN_PTR(address, SharedRuntime::dpow), getThreadTemp(), result_reg, cc->args());
      }
      break;
    case vmIntrinsics::_dsin:
      if (VM_Version::supports_sse2() && StubRoutines::dsin() != NULL) {
        __ call_runtime_leaf(StubRoutines::dsin(), getThreadTemp(), result_reg, cc->args());
      } else {
        __ call_runtime_leaf(CAST_FROM_FN_PTR(address, SharedRuntime::dsin), getThreadTemp(), result_reg, cc->args());
      }
      break;
    case vmIntrinsics::_dcos:
      if (VM_Version::supports_sse2() && StubRoutines::dcos() != NULL) {
        __ call_runtime_leaf(StubRoutines::dcos(), getThreadTemp(), result_reg, cc->args());
      } else {
        __ call_runtime_leaf(CAST_FROM_FN_PTR(address, SharedRuntime::dcos), getThreadTemp(), result_reg, cc->args());
      }
      break;
    case vmIntrinsics::_dtan:
      if (StubRoutines::dtan() != NULL) {
        __ call_runtime_leaf(StubRoutines::dtan(), getThreadTemp(), result_reg, cc->args());
      } else {
        __ call_runtime_leaf(CAST_FROM_FN_PTR(address, SharedRuntime::dtan), getThreadTemp(), result_reg, cc->args());
      }
      break;
    default:  ShouldNotReachHere();
  }
#else
  switch (x->id()) {
    case vmIntrinsics::_dexp:
      if (StubRoutines::dexp() != NULL) {
        __ call_runtime_leaf(StubRoutines::dexp(), getThreadTemp(), result_reg, cc->args());
      } else {
        __ call_runtime_leaf(CAST_FROM_FN_PTR(address, SharedRuntime::dexp), getThreadTemp(), result_reg, cc->args());
      }
      break;
    case vmIntrinsics::_dlog:
      if (StubRoutines::dlog() != NULL) {
      __ call_runtime_leaf(StubRoutines::dlog(), getThreadTemp(), result_reg, cc->args());
      } else {
        __ call_runtime_leaf(CAST_FROM_FN_PTR(address, SharedRuntime::dlog), getThreadTemp(), result_reg, cc->args());
      }
      break;
    case vmIntrinsics::_dlog10:
      if (StubRoutines::dlog10() != NULL) {
      __ call_runtime_leaf(StubRoutines::dlog10(), getThreadTemp(), result_reg, cc->args());
      } else {
        __ call_runtime_leaf(CAST_FROM_FN_PTR(address, SharedRuntime::dlog10), getThreadTemp(), result_reg, cc->args());
      }
      break;
    case vmIntrinsics::_dpow:
       if (StubRoutines::dpow() != NULL) {
      __ call_runtime_leaf(StubRoutines::dpow(), getThreadTemp(), result_reg, cc->args());
      } else {
        __ call_runtime_leaf(CAST_FROM_FN_PTR(address, SharedRuntime::dpow), getThreadTemp(), result_reg, cc->args());
      }
      break;
    case vmIntrinsics::_dsin:
      if (StubRoutines::dsin() != NULL) {
        __ call_runtime_leaf(StubRoutines::dsin(), getThreadTemp(), result_reg, cc->args());
      } else {
        __ call_runtime_leaf(CAST_FROM_FN_PTR(address, SharedRuntime::dsin), getThreadTemp(), result_reg, cc->args());
      }
      break;
    case vmIntrinsics::_dcos:
      if (StubRoutines::dcos() != NULL) {
        __ call_runtime_leaf(StubRoutines::dcos(), getThreadTemp(), result_reg, cc->args());
      } else {
        __ call_runtime_leaf(CAST_FROM_FN_PTR(address, SharedRuntime::dcos), getThreadTemp(), result_reg, cc->args());
      }
      break;
    case vmIntrinsics::_dtan:
       if (StubRoutines::dtan() != NULL) {
      __ call_runtime_leaf(StubRoutines::dtan(), getThreadTemp(), result_reg, cc->args());
      } else {
        __ call_runtime_leaf(CAST_FROM_FN_PTR(address, SharedRuntime::dtan), getThreadTemp(), result_reg, cc->args());
      }
      break;
    default:  ShouldNotReachHere();
  }
#endif // _LP64
  __ move(result_reg, calc_result);
}