int amqp_send_frame(amqp_connection_state_t state, const amqp_frame_t *frame) {
  return amqp_send_frame_inner(state, frame, AMQP_SF_NONE,
                               amqp_time_infinite());
}