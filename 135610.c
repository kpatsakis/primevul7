CtPtr ProtocolV1::read_message_footer() {
  ldout(cct, 20) << __func__ << dendl;

  state = READ_FOOTER_AND_DISPATCH;

  unsigned len;
  if (connection->has_feature(CEPH_FEATURE_MSG_AUTH)) {
    len = sizeof(ceph_msg_footer);
  } else {
    len = sizeof(ceph_msg_footer_old);
  }

  return READ(len, handle_message_footer);
}