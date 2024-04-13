void LIRGenerator::do_NegateOp(NegateOp* x) {
  LIRItem value(x->x(), this);
  value.set_destroys_register();
  value.load_item();
  LIR_Opr reg = rlock(x);

  LIR_Opr tmp = LIR_OprFact::illegalOpr;
#ifdef _LP64
  if (UseAVX > 2 && !VM_Version::supports_avx512vl()) {
    if (x->type()->tag() == doubleTag) {
      tmp = new_register(T_DOUBLE);
      __ move(LIR_OprFact::doubleConst(-0.0), tmp);
    }
    else if (x->type()->tag() == floatTag) {
      tmp = new_register(T_FLOAT);
      __ move(LIR_OprFact::floatConst(-0.0), tmp);
    }
  }
#endif
  __ negate(value.result(), reg, tmp);

  set_result(x, round_item(reg));
}