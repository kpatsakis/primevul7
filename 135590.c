CtPtr ProtocolV1::ready() {
  ldout(cct, 25) << __func__ << dendl;

  // make sure no pending tick timer
  if (connection->last_tick_id) {
    connection->center->delete_time_event(connection->last_tick_id);
  }
  connection->last_tick_id = connection->center->create_time_event(
      connection->inactive_timeout_us, connection->tick_handler);

  connection->write_lock.lock();
  can_write = WriteStatus::CANWRITE;
  if (is_queued()) {
    connection->center->dispatch_event_external(connection->write_handler);
  }
  connection->write_lock.unlock();
  connection->maybe_start_delay_thread();

  state = OPENED;
  return wait_message();
}