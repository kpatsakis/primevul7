LIR_Opr LIRGenerator::load_immediate(int x, BasicType type) {
  LIR_Opr r = NULL;
  if (type == T_LONG) {
    r = LIR_OprFact::longConst(x);
  } else if (type == T_INT) {
    r = LIR_OprFact::intConst(x);
  } else {
    ShouldNotReachHere();
  }
  return r;
}