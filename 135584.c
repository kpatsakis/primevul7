CtPtr ProtocolV1::handle_ready_connect_message_reply_write(int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << __func__ << " write ready connect message reply failed"
                  << dendl;
    return _fault();
  }

  // notify
  connection->dispatch_queue->queue_accept(connection);
  messenger->ms_deliver_handle_fast_accept(connection);
  once_ready = true;

  state = ACCEPTING_HANDLED_CONNECT_MSG;

  if (wait_for_seq) {
    return wait_seq();
  }

  return server_ready();
}