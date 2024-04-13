amqp_socket_t *amqp_get_socket(amqp_connection_state_t state) {
  return state->socket;
}