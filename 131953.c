void LIRGenerator::do_NewInstance(NewInstance* x) {
  print_if_not_loaded(x);

  CodeEmitInfo* info = state_for(x, x->state());
  LIR_Opr reg = result_register_for(x->type());
  new_instance(reg, x->klass(), x->is_unresolved(),
                       FrameMap::rcx_oop_opr,
                       FrameMap::rdi_oop_opr,
                       FrameMap::rsi_oop_opr,
                       LIR_OprFact::illegalOpr,
                       FrameMap::rdx_metadata_opr, info);
  LIR_Opr result = rlock_result(x);
  __ move(reg, result);
}