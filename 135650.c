CtPtr ProtocolV1::handle_connect_reply_1(char *buffer, int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << __func__ << " read connect reply failed" << dendl;
    return _fault();
  }

  connect_reply = *((ceph_msg_connect_reply *)buffer);

  ldout(cct, 20) << __func__ << " connect got reply tag "
                 << (int)connect_reply.tag << " connect_seq "
                 << connect_reply.connect_seq << " global_seq "
                 << connect_reply.global_seq << " proto "
                 << connect_reply.protocol_version << " flags "
                 << (int)connect_reply.flags << " features "
                 << connect_reply.features << dendl;

  if (connect_reply.authorizer_len) {
    return wait_connect_reply_auth();
  }

  return handle_connect_reply_2();
}