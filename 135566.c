void amqp_set_socket(amqp_connection_state_t state, amqp_socket_t *socket) {
  amqp_socket_delete(state->socket);
  state->socket = socket;
}