CtPtr ProtocolV1::handle_my_addr_write(int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 2) << __func__ << " connect couldn't write my addr, "
                  << cpp_strerror(r) << dendl;
    return _fault();
  }
  ldout(cct, 10) << __func__ << " connect sent my addr "
                 << messenger->get_myaddr_legacy() << dendl;

  return CONTINUE(send_connect_message);
}