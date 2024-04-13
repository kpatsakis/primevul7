LIR_Opr LIRGenerator::atomic_cmpxchg(BasicType type, LIR_Opr addr, LIRItem& cmp_value, LIRItem& new_value) {
  LIR_Opr ill = LIR_OprFact::illegalOpr;  // for convenience
  if (type == T_OBJECT || type == T_ARRAY) {
    cmp_value.load_item_force(FrameMap::rax_oop_opr);
    new_value.load_item();
#if INCLUDE_SHENANDOAHGC
    if (UseShenandoahGC) {
      __ cas_obj(addr->as_address_ptr()->base(), cmp_value.result(), new_value.result(), new_register(T_OBJECT), new_register(T_OBJECT));
    } else
#endif
    __ cas_obj(addr->as_address_ptr()->base(), cmp_value.result(), new_value.result(), ill, ill);
  } else if (type == T_INT) {
    cmp_value.load_item_force(FrameMap::rax_opr);
    new_value.load_item();
    __ cas_int(addr->as_address_ptr()->base(), cmp_value.result(), new_value.result(), ill, ill);
  } else if (type == T_LONG) {
    cmp_value.load_item_force(FrameMap::long0_opr);
    new_value.load_item_force(FrameMap::long1_opr);
    __ cas_long(addr->as_address_ptr()->base(), cmp_value.result(), new_value.result(), ill, ill);
  } else {
    Unimplemented();
  }
  LIR_Opr result = new_register(T_INT);
  __ cmove(lir_cond_equal, LIR_OprFact::intConst(1), LIR_OprFact::intConst(0),
           result, type);
  return result;
}