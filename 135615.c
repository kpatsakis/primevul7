CtPtr ProtocolV1::handle_connect_message_reply_write(int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << " write connect message reply failed" << dendl;
    connection->inject_delay();
    return _fault();
  }

  return CONTINUE(wait_connect_message);
}