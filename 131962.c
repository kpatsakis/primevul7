LIR_Opr LIRGenerator::result_register_for(ValueType* type, bool callee) {
  LIR_Opr opr;
  switch (type->tag()) {
    case intTag:     opr = FrameMap::rax_opr;          break;
    case objectTag:  opr = FrameMap::rax_oop_opr;      break;
    case longTag:    opr = FrameMap::long0_opr;        break;
    case floatTag:   opr = UseSSE >= 1 ? FrameMap::xmm0_float_opr  : FrameMap::fpu0_float_opr;  break;
    case doubleTag:  opr = UseSSE >= 2 ? FrameMap::xmm0_double_opr : FrameMap::fpu0_double_opr;  break;

    case addressTag:
    default: ShouldNotReachHere(); return LIR_OprFact::illegalOpr;
  }

  assert(opr->type_field() == as_OprType(as_BasicType(type)), "type mismatch");
  return opr;
}