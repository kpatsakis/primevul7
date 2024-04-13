int amqp_tune_connection(amqp_connection_state_t state, int channel_max,
                         int frame_max, int heartbeat) {
  void *newbuf;
  int res;

  ENFORCE_STATE(state, CONNECTION_STATE_IDLE);

  state->channel_max = channel_max;
  state->frame_max = frame_max;

  state->heartbeat = heartbeat;
  if (0 > state->heartbeat) {
    state->heartbeat = 0;
  }

  res = amqp_time_s_from_now(&state->next_send_heartbeat,
                             amqp_heartbeat_send(state));
  if (AMQP_STATUS_OK != res) {
    return res;
  }
  res = amqp_time_s_from_now(&state->next_recv_heartbeat,
                             amqp_heartbeat_recv(state));
  if (AMQP_STATUS_OK != res) {
    return res;
  }

  state->outbound_buffer.len = frame_max;
  newbuf = realloc(state->outbound_buffer.bytes, frame_max);
  if (newbuf == NULL) {
    return AMQP_STATUS_NO_MEMORY;
  }
  state->outbound_buffer.bytes = newbuf;

  return AMQP_STATUS_OK;
}