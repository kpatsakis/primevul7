amqp_boolean_t amqp_release_buffers_ok(amqp_connection_state_t state) {
  return (state->state == CONNECTION_STATE_IDLE);
}