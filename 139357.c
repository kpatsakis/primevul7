static gg_action_t gg_handle_sending_hub_proxy(struct gg_session *sess, struct gg_event *e, enum gg_state_t next_state, enum gg_state_t alt_state, enum gg_state_t alt2_state)
{
	if (gg_send_queued_data(sess) == -1) {
		e->event.failure = GG_FAILURE_WRITING;
		return GG_ACTION_FAIL;
	}

	if (sess->send_left > 0)
		return GG_ACTION_WAIT;

	sess->state = next_state;
	sess->check = GG_CHECK_READ;
	sess->timeout = GG_DEFAULT_TIMEOUT;

	return GG_ACTION_WAIT;
}