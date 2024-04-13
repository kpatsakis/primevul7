void amqp_release_buffers(amqp_connection_state_t state) {
  int i;
  ENFORCE_STATE(state, CONNECTION_STATE_IDLE);

  for (i = 0; i < POOL_TABLE_SIZE; ++i) {
    amqp_pool_table_entry_t *entry = state->pool_table[i];

    for (; NULL != entry; entry = entry->next) {
      amqp_maybe_release_buffers_on_channel(state, entry->channel);
    }
  }
}