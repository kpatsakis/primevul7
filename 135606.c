CtPtr ProtocolV1::send_client_banner() {
  ldout(cct, 20) << __func__ << dendl;
  state = CONNECTING;

  bufferlist bl;
  bl.append(CEPH_BANNER, strlen(CEPH_BANNER));
  return WRITE(bl, handle_client_banner_write);
}