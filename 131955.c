LIR_Opr LIRGenerator::getThreadPointer() {
#ifdef _LP64
  return FrameMap::as_pointer_opr(r15_thread);
#else
  LIR_Opr result = new_register(T_INT);
  __ get_thread(result);
  return result;
#endif //
}