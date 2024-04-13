CtPtr ProtocolV1::handle_ack_seq(char *buffer, int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << __func__ << " read connect ack seq failed" << dendl;
    return _fault();
  }

  uint64_t newly_acked_seq = 0;

  newly_acked_seq = *((uint64_t *)buffer);
  ldout(cct, 2) << __func__ << " got newly_acked_seq " << newly_acked_seq
                << " vs out_seq " << out_seq << dendl;
  out_seq = discard_requeued_up_to(out_seq, newly_acked_seq);

  bufferlist bl;
  uint64_t s = in_seq;
  bl.append((char *)&s, sizeof(s));

  return WRITE(bl, handle_in_seq_write);
}