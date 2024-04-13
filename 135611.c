CtPtr ProtocolV1::handle_connect_message_auth(char *buffer, int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << __func__ << " read connect authorizer failed" << dendl;
    return _fault();
  }

  return handle_connect_message_2();
}