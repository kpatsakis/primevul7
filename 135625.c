CtPtr ProtocolV1::handle_seq(char *buffer, int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << __func__ << " read ack seq failed" << dendl;
    return _fault();
  }

  uint64_t newly_acked_seq = *(uint64_t *)buffer;
  ldout(cct, 2) << __func__ << " accept get newly_acked_seq " << newly_acked_seq
                << dendl;
  out_seq = discard_requeued_up_to(out_seq, newly_acked_seq);

  return server_ready();
}