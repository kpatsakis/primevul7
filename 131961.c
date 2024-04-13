LIR_Opr LIRGenerator::atomic_xchg(BasicType type, LIR_Opr addr, LIRItem& value) {
  bool is_oop = type == T_OBJECT || type == T_ARRAY;
  LIR_Opr result = new_register(type);
  value.load_item();
  // Because we want a 2-arg form of xchg and xadd
  __ move(value.result(), result);
  assert(type == T_INT || is_oop LP64_ONLY( || type == T_LONG ), "unexpected type");
#if INCLUDE_SHENANDOAHGC
  if (UseShenandoahGC) {
    LIR_Opr tmp = is_oop ? new_register(type) : LIR_OprFact::illegalOpr;
    __ xchg(addr, result, result, tmp);
  } else
#endif
  __ xchg(addr, result, result, LIR_OprFact::illegalOpr);
  return result;
}