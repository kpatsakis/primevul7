CtPtr ProtocolV1::handle_client_banner(char *buffer, int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << __func__ << " read peer banner and addr failed" << dendl;
    return _fault();
  }

  if (memcmp(buffer, CEPH_BANNER, strlen(CEPH_BANNER))) {
    ldout(cct, 1) << __func__ << " accept peer sent bad banner '" << buffer
                  << "' (should be '" << CEPH_BANNER << "')" << dendl;
    return _fault();
  }

  bufferlist addr_bl;
  entity_addr_t peer_addr;

  addr_bl.append(buffer + strlen(CEPH_BANNER), sizeof(ceph_entity_addr));
  try {
    auto ti = addr_bl.cbegin();
    decode(peer_addr, ti);
  } catch (const buffer::error &e) {
    lderr(cct) << __func__ << " decode peer_addr failed " << dendl;
    return _fault();
  }

  ldout(cct, 10) << __func__ << " accept peer addr is " << peer_addr << dendl;
  if (peer_addr.is_blank_ip()) {
    // peer apparently doesn't know what ip they have; figure it out for them.
    int port = peer_addr.get_port();
    peer_addr.set_sockaddr(connection->target_addr.get_sockaddr());
    peer_addr.set_port(port);

    ldout(cct, 0) << __func__ << " accept peer addr is really " << peer_addr
                  << " (socket is " << connection->target_addr << ")" << dendl;
  }
  connection->set_peer_addr(peer_addr);  // so that connection_state gets set up
  connection->target_addr = peer_addr;

  return CONTINUE(wait_connect_message);
}