void ProtocolV1::append_keepalive_or_ack(bool ack, utime_t *tp) {
  ldout(cct, 10) << __func__ << dendl;
  if (ack) {
    ceph_assert(tp);
    struct ceph_timespec ts;
    tp->encode_timeval(&ts);
    connection->outgoing_bl.append(CEPH_MSGR_TAG_KEEPALIVE2_ACK);
    connection->outgoing_bl.append((char *)&ts, sizeof(ts));
  } else if (connection->has_feature(CEPH_FEATURE_MSGR_KEEPALIVE2)) {
    struct ceph_timespec ts;
    utime_t t = ceph_clock_now();
    t.encode_timeval(&ts);
    connection->outgoing_bl.append(CEPH_MSGR_TAG_KEEPALIVE2);
    connection->outgoing_bl.append((char *)&ts, sizeof(ts));
  } else {
    connection->outgoing_bl.append(CEPH_MSGR_TAG_KEEPALIVE);
  }
}