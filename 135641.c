CtPtr ProtocolV1::handle_server_banner_write(int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << " write server banner failed" << dendl;
    return _fault();
  }
  ldout(cct, 10) << __func__ << " write banner and addr done: "
                 << connection->get_peer_addr() << dendl;

  return wait_client_banner();
}