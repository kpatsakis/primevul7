CtPtr ProtocolV1::wait_connect_reply_auth() {
  ldout(cct, 20) << __func__ << dendl;

  ldout(cct, 10) << __func__
                 << " reply.authorizer_len=" << connect_reply.authorizer_len
                 << dendl;

  ceph_assert(connect_reply.authorizer_len < 4096);

  return READ(connect_reply.authorizer_len, handle_connect_reply_auth);
}