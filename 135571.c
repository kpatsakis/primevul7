int amqp_get_sockfd(amqp_connection_state_t state) {
  return state->socket ? amqp_socket_get_sockfd(state->socket) : -1;
}