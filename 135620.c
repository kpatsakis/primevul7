void ProtocolV1::fault() {
  ldout(cct, 20) << __func__ << dendl;

  if (state == CLOSED || state == NONE) {
    ldout(cct, 10) << __func__ << " connection is already closed" << dendl;
    return;
  }

  if (connection->policy.lossy && state != START_CONNECT &&
      state != CONNECTING) {
    ldout(cct, 1) << __func__ << " on lossy channel, failing" << dendl;
    stop();
    connection->dispatch_queue->queue_reset(connection);
    return;
  }

  connection->write_lock.lock();
  can_write = WriteStatus::NOWRITE;
  is_reset_from_peer = false;

  // requeue sent items
  requeue_sent();

  if (!once_ready && out_q.empty() && state >= START_ACCEPT &&
      state <= ACCEPTING_WAIT_CONNECT_MSG_AUTH && !replacing) {
    ldout(cct, 10) << __func__ << " with nothing to send and in the half "
                   << " accept state just closed" << dendl;
    connection->write_lock.unlock();
    stop();
    connection->dispatch_queue->queue_reset(connection);
    return;
  }
  replacing = false;

  connection->fault();

  reset_recv_state();

  if (connection->policy.standby && out_q.empty() && !keepalive &&
      state != WAIT) {
    ldout(cct, 10) << __func__ << " with nothing to send, going to standby"
                   << dendl;
    state = STANDBY;
    connection->write_lock.unlock();
    return;
  }

  connection->write_lock.unlock();

  if ((state >= START_CONNECT && state <= CONNECTING_SEND_CONNECT_MSG) ||
      state == WAIT) {
    // backoff!
    if (state == WAIT) {
      backoff.set_from_double(cct->_conf->ms_max_backoff);
    } else if (backoff == utime_t()) {
      backoff.set_from_double(cct->_conf->ms_initial_backoff);
    } else {
      backoff += backoff;
      if (backoff > cct->_conf->ms_max_backoff)
        backoff.set_from_double(cct->_conf->ms_max_backoff);
    }

    global_seq = messenger->get_global_seq();
    state = START_CONNECT;
    connection->state = AsyncConnection::STATE_CONNECTING;
    ldout(cct, 10) << __func__ << " waiting " << backoff << dendl;
    // woke up again;
    connection->register_time_events.insert(
        connection->center->create_time_event(backoff.to_nsec() / 1000,
                                              connection->wakeup_handler));
  } else {
    // policy maybe empty when state is in accept
    if (connection->policy.server) {
      ldout(cct, 0) << __func__ << " server, going to standby" << dendl;
      state = STANDBY;
    } else {
      ldout(cct, 0) << __func__ << " initiating reconnect" << dendl;
      connect_seq++;
      global_seq = messenger->get_global_seq();
      state = START_CONNECT;
      connection->state = AsyncConnection::STATE_CONNECTING;
    }
    backoff = utime_t();
    connection->center->dispatch_event_external(connection->read_handler);
  }
}