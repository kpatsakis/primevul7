void ProtocolV1::session_reset() {
  ldout(cct, 10) << __func__ << " started" << dendl;

  std::lock_guard<std::mutex> l(connection->write_lock);
  if (connection->delay_state) {
    connection->delay_state->discard();
  }

  connection->dispatch_queue->discard_queue(connection->conn_id);
  discard_out_queue();
  // note: we need to clear outgoing_bl here, but session_reset may be
  // called by other thread, so let caller clear this itself!
  // outgoing_bl.clear();

  connection->dispatch_queue->queue_remote_reset(connection);

  randomize_out_seq();

  in_seq = 0;
  connect_seq = 0;
  // it's safe to directly set 0, double locked
  ack_left = 0;
  once_ready = false;
  can_write = WriteStatus::NOWRITE;
}