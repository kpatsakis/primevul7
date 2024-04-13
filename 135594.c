CtPtr ProtocolV1::handle_server_banner_and_identify(char *buffer, int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << __func__ << " read banner and identify addresses failed"
                  << dendl;
    return _fault();
  }

  unsigned banner_len = strlen(CEPH_BANNER);
  if (memcmp(buffer, CEPH_BANNER, banner_len)) {
    ldout(cct, 0) << __func__ << " connect protocol error (bad banner) on peer "
                  << connection->get_peer_addr() << dendl;
    return _fault();
  }

  bufferlist bl;
  entity_addr_t paddr, peer_addr_for_me;

  bl.append(buffer + banner_len, sizeof(ceph_entity_addr) * 2);
  auto p = bl.cbegin();
  try {
    decode(paddr, p);
    decode(peer_addr_for_me, p);
  } catch (const buffer::error &e) {
    lderr(cct) << __func__ << " decode peer addr failed " << dendl;
    return _fault();
  }
  ldout(cct, 20) << __func__ << " connect read peer addr " << paddr
                 << " on socket " << connection->cs.fd() << dendl;

  entity_addr_t peer_addr = connection->peer_addrs->legacy_addr();
  if (peer_addr != paddr) {
    if (paddr.is_blank_ip() && peer_addr.get_port() == paddr.get_port() &&
        peer_addr.get_nonce() == paddr.get_nonce()) {
      ldout(cct, 0) << __func__ << " connect claims to be " << paddr << " not "
                    << peer_addr << " - presumably this is the same node!"
                    << dendl;
    } else {
      ldout(cct, 10) << __func__ << " connect claims to be " << paddr << " not "
                     << peer_addr << dendl;
      return _fault();
    }
  }

  ldout(cct, 20) << __func__ << " connect peer addr for me is "
                 << peer_addr_for_me << dendl;
  if (messenger->get_myaddrs().empty() ||
      messenger->get_myaddrs().front().is_blank_ip()) {
    sockaddr_storage ss;
    socklen_t len = sizeof(ss);
    getsockname(connection->cs.fd(), (sockaddr *)&ss, &len);
    entity_addr_t a;
    if (cct->_conf->ms_learn_addr_from_peer) {
      ldout(cct, 1) << __func__ << " peer " << connection->target_addr
		    << " says I am " << peer_addr_for_me << " (socket says "
		    << (sockaddr*)&ss << ")" << dendl;
      a = peer_addr_for_me;
    } else {
      ldout(cct, 1) << __func__ << " socket to  " << connection->target_addr
		    << " says I am " << (sockaddr*)&ss
		    << " (peer says " << peer_addr_for_me << ")" << dendl;
      a.set_sockaddr((sockaddr *)&ss);
    }
    a.set_type(entity_addr_t::TYPE_LEGACY); // anything but NONE; learned_addr ignores this
    a.set_port(0);
    connection->lock.unlock();
    messenger->learned_addr(a);
    if (cct->_conf->ms_inject_internal_delays &&
	cct->_conf->ms_inject_socket_failures) {
      if (rand() % cct->_conf->ms_inject_socket_failures == 0) {
	ldout(cct, 10) << __func__ << " sleep for "
		       << cct->_conf->ms_inject_internal_delays << dendl;
	utime_t t;
	t.set_from_double(cct->_conf->ms_inject_internal_delays);
	t.sleep();
      }
    }
    connection->lock.lock();
    if (state != CONNECTING_WAIT_BANNER_AND_IDENTIFY) {
      ldout(cct, 1) << __func__
                  << " state changed while learned_addr, mark_down or "
		    << " replacing must be happened just now" << dendl;
      return nullptr;
    }
  }

  bufferlist myaddrbl;
  encode(messenger->get_myaddr_legacy(), myaddrbl, 0);  // legacy
  return WRITE(myaddrbl, handle_my_addr_write);
}