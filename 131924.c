LIR_Address* LIRGenerator::emit_array_address(LIR_Opr array_opr, LIR_Opr index_opr,
                                              BasicType type) {
  int offset_in_bytes = arrayOopDesc::base_offset_in_bytes(type);

  LIR_Address* addr;
  if (index_opr->is_constant()) {
    int elem_size = type2aelembytes(type);
#ifdef _LP64
    jint index = index_opr->as_jint();
    jlong disp = offset_in_bytes + (jlong)(index) * elem_size;
    if (disp > max_jint) {
      // Displacement overflow. Cannot directly use instruction with 32-bit displacement for 64-bit addresses.
      // Convert array index to long to do array offset computation with 64-bit values.
      index_opr = new_register(T_LONG);
      __ move(LIR_OprFact::longConst(index), index_opr);
      addr = new LIR_Address(array_opr, index_opr, LIR_Address::scale(type), offset_in_bytes, type);
    } else {
      addr = new LIR_Address(array_opr, (intx)disp, type);
    }
#else
    // A displacement overflow can also occur for x86 but that is not a problem due to the 32-bit address range!
    // Let's assume an array 'a' and an access with displacement 'disp'. When disp overflows, then "a + disp" will
    // always be negative (i.e. underflows the 32-bit address range):
    // Let N = 2^32: a + signed_overflow(disp) = a + disp - N.
    // "a + disp" is always smaller than N. If an index was chosen which would point to an address beyond N, then
    // range checks would catch that and throw an exception. Thus, a + disp < 0 holds which means that it always
    // underflows the 32-bit address range:
    // unsigned_underflow(a + signed_overflow(disp)) = unsigned_underflow(a + disp - N)
    //                                              = (a + disp - N) + N = a + disp
    // This shows that we still end up at the correct address with a displacement overflow due to the 32-bit address
    // range limitation. This overflow only needs to be handled if addresses can be larger as on 64-bit platforms.
    addr = new LIR_Address(array_opr, offset_in_bytes + (intx)(index_opr->as_jint()) * elem_size, type);
#endif // _LP64
  } else {
#ifdef _LP64
    if (index_opr->type() == T_INT) {
      LIR_Opr tmp = new_register(T_LONG);
      __ convert(Bytecodes::_i2l, index_opr, tmp);
      index_opr = tmp;
    }
#endif // _LP64
    addr =  new LIR_Address(array_opr,
                            index_opr,
                            LIR_Address::scale(type),
                            offset_in_bytes, type);
  }
  return addr;
}