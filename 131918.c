void LIRGenerator::do_vectorizedMismatch(Intrinsic* x) {
  assert(UseVectorizedMismatchIntrinsic, "need AVX instruction support");

  // Make all state_for calls early since they can emit code
  LIR_Opr result = rlock_result(x);

  LIRItem a(x->argument_at(0), this); // Object
  LIRItem aOffset(x->argument_at(1), this); // long
  LIRItem b(x->argument_at(2), this); // Object
  LIRItem bOffset(x->argument_at(3), this); // long
  LIRItem length(x->argument_at(4), this); // int
  LIRItem log2ArrayIndexScale(x->argument_at(5), this); // int

  a.load_item();
  aOffset.load_nonconstant();
  b.load_item();
  bOffset.load_nonconstant();

  long constant_aOffset = 0;
  LIR_Opr result_aOffset = aOffset.result();
  if (result_aOffset->is_constant()) {
    constant_aOffset = result_aOffset->as_jlong();
    result_aOffset = LIR_OprFact::illegalOpr;
  }
  LIR_Opr result_a = a.result();

  long constant_bOffset = 0;
  LIR_Opr result_bOffset = bOffset.result();
  if (result_bOffset->is_constant()) {
    constant_bOffset = result_bOffset->as_jlong();
    result_bOffset = LIR_OprFact::illegalOpr;
  }
  LIR_Opr result_b = b.result();

#ifndef _LP64
  result_a = new_register(T_INT);
  __ convert(Bytecodes::_l2i, a.result(), result_a);
  result_b = new_register(T_INT);
  __ convert(Bytecodes::_l2i, b.result(), result_b);
#endif


  LIR_Address* addr_a = new LIR_Address(result_a,
                                        result_aOffset,
                                        constant_aOffset,
                                        T_BYTE);

  LIR_Address* addr_b = new LIR_Address(result_b,
                                        result_bOffset,
                                        constant_bOffset,
                                        T_BYTE);

  BasicTypeList signature(4);
  signature.append(T_ADDRESS);
  signature.append(T_ADDRESS);
  signature.append(T_INT);
  signature.append(T_INT);
  CallingConvention* cc = frame_map()->c_calling_convention(&signature);
  const LIR_Opr result_reg = result_register_for(x->type());

  LIR_Opr ptr_addr_a = new_pointer_register();
  __ leal(LIR_OprFact::address(addr_a), ptr_addr_a);

  LIR_Opr ptr_addr_b = new_pointer_register();
  __ leal(LIR_OprFact::address(addr_b), ptr_addr_b);

  __ move(ptr_addr_a, cc->at(0));
  __ move(ptr_addr_b, cc->at(1));
  length.load_item_force(cc->at(2));
  log2ArrayIndexScale.load_item_force(cc->at(3));

  __ call_runtime_leaf(StubRoutines::vectorizedMismatch(), getThreadTemp(), result_reg, cc->args());
  __ move(result_reg, result);
}