void LIRGenerator::do_MathIntrinsic(Intrinsic* x) {
  assert(x->number_of_arguments() == 1 || (x->number_of_arguments() == 2 && x->id() == vmIntrinsics::_dpow), "wrong type");

  if (x->id() == vmIntrinsics::_dexp || x->id() == vmIntrinsics::_dlog ||
      x->id() == vmIntrinsics::_dpow || x->id() == vmIntrinsics::_dcos ||
      x->id() == vmIntrinsics::_dsin || x->id() == vmIntrinsics::_dtan ||
      x->id() == vmIntrinsics::_dlog10) {
    do_LibmIntrinsic(x);
    return;
  }

  LIRItem value(x->argument_at(0), this);

  bool use_fpu = false;
  if (UseSSE < 2) {
    value.set_destroys_register();
  }
  value.load_item();

  LIR_Opr calc_input = value.result();
  LIR_Opr calc_result = rlock_result(x);

  LIR_Opr tmp = LIR_OprFact::illegalOpr;
#ifdef _LP64
  if (UseAVX > 2 && (!VM_Version::supports_avx512vl()) &&
      (x->id() == vmIntrinsics::_dabs)) {
    tmp = new_register(T_DOUBLE);
    __ move(LIR_OprFact::doubleConst(-0.0), tmp);
  }
#endif

  switch(x->id()) {
    case vmIntrinsics::_dabs:   __ abs  (calc_input, calc_result, tmp); break;
    case vmIntrinsics::_dsqrt:  __ sqrt (calc_input, calc_result, LIR_OprFact::illegalOpr); break;
    default:                    ShouldNotReachHere();
  }

  if (use_fpu) {
    __ move(calc_result, x->operand());
  }
}