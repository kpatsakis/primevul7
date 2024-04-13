CtPtr ProtocolV1::handle_keepalive2(char *buffer, int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << __func__ << " read keeplive timespec failed" << dendl;
    return _fault();
  }

  ldout(cct, 30) << __func__ << " got KEEPALIVE2 tag ..." << dendl;

  ceph_timespec *t;
  t = (ceph_timespec *)buffer;
  utime_t kp_t = utime_t(*t);
  connection->write_lock.lock();
  append_keepalive_or_ack(true, &kp_t);
  connection->write_lock.unlock();

  ldout(cct, 20) << __func__ << " got KEEPALIVE2 " << kp_t << dendl;
  connection->set_last_keepalive(ceph_clock_now());

  if (is_connected()) {
    connection->center->dispatch_event_external(connection->write_handler);
  }

  return CONTINUE(wait_message);
}