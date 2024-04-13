CtPtr ProtocolV1::wait_ack_seq() {
  ldout(cct, 20) << __func__ << dendl;

  return READ(sizeof(uint64_t), handle_ack_seq);
}