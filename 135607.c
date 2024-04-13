CtPtr ProtocolV1::handle_connect_message_1(char *buffer, int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << __func__ << " read connect msg failed" << dendl;
    return _fault();
  }

  connect_msg = *((ceph_msg_connect *)buffer);

  state = ACCEPTING_WAIT_CONNECT_MSG_AUTH;

  if (connect_msg.authorizer_len) {
    return wait_connect_message_auth();
  }

  return handle_connect_message_2();
}