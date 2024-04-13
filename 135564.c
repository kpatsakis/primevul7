int amqp_get_frame_max(amqp_connection_state_t state) {
  return state->frame_max;
}