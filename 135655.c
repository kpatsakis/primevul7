CtPtr ProtocolV1::send_server_banner() {
  ldout(cct, 20) << __func__ << dendl;
  state = ACCEPTING;

  bufferlist bl;

  bl.append(CEPH_BANNER, strlen(CEPH_BANNER));

  // as a server, we should have a legacy addr if we accepted this connection.
  auto legacy = messenger->get_myaddrs().legacy_addr();
  encode(legacy, bl, 0);  // legacy
  connection->port = legacy.get_port();
  encode(connection->target_addr, bl, 0);  // legacy

  ldout(cct, 1) << __func__ << " sd=" << connection->cs.fd()
		<< " legacy " << legacy
		<< " socket_addr " << connection->socket_addr
		<< " target_addr " << connection->target_addr
		<< dendl;

  return WRITE(bl, handle_server_banner_write);
}