CtPtr ProtocolV1::wait_seq() {
  ldout(cct, 20) << __func__ << dendl;

  return READ(sizeof(uint64_t), handle_seq);
}