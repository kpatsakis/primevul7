int amqp_destroy_connection(amqp_connection_state_t state) {
  int status = AMQP_STATUS_OK;
  if (state) {
    int i;
    for (i = 0; i < POOL_TABLE_SIZE; ++i) {
      amqp_pool_table_entry_t *entry = state->pool_table[i];
      while (NULL != entry) {
        amqp_pool_table_entry_t *todelete = entry;
        empty_amqp_pool(&entry->pool);
        entry = entry->next;
        free(todelete);
      }
    }

    free(state->outbound_buffer.bytes);
    free(state->sock_inbound_buffer.bytes);
    amqp_socket_delete(state->socket);
    empty_amqp_pool(&state->properties_pool);
    free(state);
  }
  return status;
}