CtPtr ProtocolV1::handle_client_banner_write(int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << __func__ << " write client banner failed" << dendl;
    return _fault();
  }
  ldout(cct, 10) << __func__ << " connect write banner done: "
                 << connection->get_peer_addr() << dendl;

  return wait_server_banner();
}