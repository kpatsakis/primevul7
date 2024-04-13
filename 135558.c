static size_t consume_data(amqp_connection_state_t state,
                           amqp_bytes_t *received_data) {
  /* how much data is available and will fit? */
  size_t bytes_consumed = state->target_size - state->inbound_offset;
  if (received_data->len < bytes_consumed) {
    bytes_consumed = received_data->len;
  }

  memcpy(amqp_offset(state->inbound_buffer.bytes, state->inbound_offset),
         received_data->bytes, bytes_consumed);
  state->inbound_offset += bytes_consumed;
  received_data->bytes = amqp_offset(received_data->bytes, bytes_consumed);
  received_data->len -= bytes_consumed;

  return bytes_consumed;
}