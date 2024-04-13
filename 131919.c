void LIRGenerator::array_store_check(LIR_Opr value, LIR_Opr array, CodeEmitInfo* store_check_info, ciMethod* profiled_method, int profiled_bci) {
  LIR_Opr tmp1 = new_register(objectType);
  LIR_Opr tmp2 = new_register(objectType);
  LIR_Opr tmp3 = new_register(objectType);
  __ store_check(value, array, tmp1, tmp2, tmp3, store_check_info, profiled_method, profiled_bci);
}