CtPtr ProtocolV1::wait_server_banner() {
  state = CONNECTING_WAIT_BANNER_AND_IDENTIFY;

  ldout(cct, 20) << __func__ << dendl;

  bufferlist myaddrbl;
  unsigned banner_len = strlen(CEPH_BANNER);
  unsigned need_len = banner_len + sizeof(ceph_entity_addr) * 2;
  return READ(need_len, handle_server_banner_and_identify);
}