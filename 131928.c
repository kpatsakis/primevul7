LIR_Opr fixed_register_for(BasicType type) {
  switch (type) {
    case T_FLOAT:  return FrameMap::fpu0_float_opr;
    case T_DOUBLE: return FrameMap::fpu0_double_opr;
    case T_INT:    return FrameMap::rax_opr;
    case T_LONG:   return FrameMap::long0_opr;
    default:       ShouldNotReachHere(); return LIR_OprFact::illegalOpr;
  }
}