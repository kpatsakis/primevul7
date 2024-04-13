CtPtr ProtocolV1::wait_connect_reply() {
  ldout(cct, 20) << __func__ << dendl;

  // FIPS zeroization audit 20191115: this memset is not security related.
  memset(&connect_reply, 0, sizeof(connect_reply));
  return READ(sizeof(connect_reply), handle_connect_reply_1);
}