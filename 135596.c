CtPtr ProtocolV1::wait_client_banner() {
  ldout(cct, 20) << __func__ << dendl;

  return READ(strlen(CEPH_BANNER) + sizeof(ceph_entity_addr),
              handle_client_banner);
}