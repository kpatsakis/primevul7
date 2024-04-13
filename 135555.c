amqp_table_t *amqp_get_client_properties(amqp_connection_state_t state) {
  return &state->client_properties;
}