int amqp_get_heartbeat(amqp_connection_state_t state) {
  return state->heartbeat;
}