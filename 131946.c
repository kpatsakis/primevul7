LIR_Opr LIRGenerator::safepoint_poll_register() {
  NOT_LP64( if (SafepointMechanism::uses_thread_local_poll()) { return new_register(T_ADDRESS); } )
  return LIR_OprFact::illegalOpr;
}