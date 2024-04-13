static void binder_cleanup_transaction(struct binder_transaction *t,
				       const char *reason,
				       uint32_t error_code)
{
	if (t->buffer->target_node && !(t->flags & TF_ONE_WAY)) {
		binder_send_failed_reply(t, error_code);
	} else {
		binder_debug(BINDER_DEBUG_DEAD_TRANSACTION,
			"undelivered transaction %d, %s\n",
			t->debug_id, reason);
		binder_free_transaction(t);
	}
}