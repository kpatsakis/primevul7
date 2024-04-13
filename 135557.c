amqp_table_t *amqp_get_server_properties(amqp_connection_state_t state) {
  return &state->server_properties;
}