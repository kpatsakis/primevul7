void amqp_maybe_release_buffers_on_channel(amqp_connection_state_t state,
                                           amqp_channel_t channel) {
  amqp_link_t *queued_link;
  amqp_pool_t *pool;
  if (CONNECTION_STATE_IDLE != state->state) {
    return;
  }

  queued_link = state->first_queued_frame;

  while (NULL != queued_link) {
    amqp_frame_t *frame = queued_link->data;
    if (channel == frame->channel) {
      return;
    }

    queued_link = queued_link->next;
  }

  pool = amqp_get_channel_pool(state, channel);

  if (pool != NULL) {
    recycle_amqp_pool(pool);
  }
}