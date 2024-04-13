void ProtocolV1::stop() {
  ldout(cct, 20) << __func__ << dendl;
  if (state == CLOSED) {
    return;
  }

  if (connection->delay_state) connection->delay_state->flush();

  ldout(cct, 2) << __func__ << dendl;
  std::lock_guard<std::mutex> l(connection->write_lock);

  reset_recv_state();
  discard_out_queue();

  connection->_stop();

  can_write = WriteStatus::CLOSED;
  state = CLOSED;
}