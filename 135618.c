CtPtr ProtocolV1::wait_connect_message_auth() {
  ldout(cct, 20) << __func__ << dendl;
  authorizer_buf.clear();
  authorizer_buf.push_back(buffer::create(connect_msg.authorizer_len));
  return READB(connect_msg.authorizer_len, authorizer_buf.c_str(),
               handle_connect_message_auth);
}