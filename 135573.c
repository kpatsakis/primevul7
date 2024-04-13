int amqp_get_channel_max(amqp_connection_state_t state) {
  return state->channel_max;
}