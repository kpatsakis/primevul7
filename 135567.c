static void return_to_idle(amqp_connection_state_t state) {
  state->inbound_buffer.len = sizeof(state->header_buffer);
  state->inbound_buffer.bytes = state->header_buffer;
  state->inbound_offset = 0;
  state->target_size = HEADER_SIZE;
  state->state = CONNECTION_STATE_IDLE;
}