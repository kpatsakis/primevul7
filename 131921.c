LIR_Address* LIRGenerator::generate_address(LIR_Opr base, LIR_Opr index,
                                            int shift, int disp, BasicType type) {
  assert(base->is_register(), "must be");
  if (index->is_constant()) {
    LIR_Const *constant = index->as_constant_ptr();
#ifdef _LP64
    jlong c;
    if (constant->type() == T_INT) {
      c = (jlong(index->as_jint()) << shift) + disp;
    } else {
      assert(constant->type() == T_LONG, "should be");
      c = (index->as_jlong() << shift) + disp;
    }
    if ((jlong)((jint)c) == c) {
      return new LIR_Address(base, (jint)c, type);
    } else {
      LIR_Opr tmp = new_register(T_LONG);
      __ move(index, tmp);
      return new LIR_Address(base, tmp, type);
    }
#else
    return new LIR_Address(base,
                           ((intx)(constant->as_jint()) << shift) + disp,
                           type);
#endif
  } else {
    return new LIR_Address(base, index, (LIR_Address::Scale)shift, disp, type);
  }
}