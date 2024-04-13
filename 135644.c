CtPtr ProtocolV1::handle_keepalive2_ack(char *buffer, int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << __func__ << " read keeplive timespec failed" << dendl;
    return _fault();
  }

  ceph_timespec *t;
  t = (ceph_timespec *)buffer;
  connection->set_last_keepalive_ack(utime_t(*t));
  ldout(cct, 20) << __func__ << " got KEEPALIVE_ACK" << dendl;

  return CONTINUE(wait_message);
}