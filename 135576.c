amqp_connection_state_t amqp_new_connection(void) {
  int res;
  amqp_connection_state_t state = (amqp_connection_state_t)calloc(
      1, sizeof(struct amqp_connection_state_t_));

  if (state == NULL) {
    return NULL;
  }

  res = amqp_tune_connection(state, 0, AMQP_INITIAL_FRAME_POOL_PAGE_SIZE, 0);
  if (0 != res) {
    goto out_nomem;
  }

  state->inbound_buffer.bytes = state->header_buffer;
  state->inbound_buffer.len = sizeof(state->header_buffer);

  state->state = CONNECTION_STATE_INITIAL;
  /* the server protocol version response is 8 bytes, which conveniently
     is also the minimum frame size */
  state->target_size = 8;

  state->sock_inbound_buffer.len = AMQP_INITIAL_INBOUND_SOCK_BUFFER_SIZE;
  state->sock_inbound_buffer.bytes =
      malloc(AMQP_INITIAL_INBOUND_SOCK_BUFFER_SIZE);
  if (state->sock_inbound_buffer.bytes == NULL) {
    goto out_nomem;
  }

  init_amqp_pool(&state->properties_pool, 512);

  /* Use address of the internal_handshake_timeout object by default. */
  state->internal_handshake_timeout.tv_sec = AMQP_DEFAULT_LOGIN_TIMEOUT_SEC;
  state->internal_handshake_timeout.tv_usec = 0;
  state->handshake_timeout = &state->internal_handshake_timeout;

  return state;

out_nomem:
  free(state->sock_inbound_buffer.bytes);
  free(state);
  return NULL;
}