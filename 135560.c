void amqp_set_sockfd(amqp_connection_state_t state, int sockfd) {
  amqp_socket_t *socket = amqp_tcp_socket_new(state);
  if (!socket) {
    amqp_abort("%s", strerror(errno));
  }
  amqp_tcp_socket_set_sockfd(socket, sockfd);
}