CtPtr ProtocolV1::handle_in_seq_write(int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 10) << __func__ << " failed to send in_seq " << dendl;
    return _fault();
  }

  ldout(cct, 10) << __func__ << " send in_seq done " << dendl;

  return client_ready();
}