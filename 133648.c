
static void busy_poll_stop(struct napi_struct *napi, void *have_poll_lock)
{
	int rc;

	/* Busy polling means there is a high chance device driver hard irq
	 * could not grab NAPI_STATE_SCHED, and that NAPI_STATE_MISSED was
	 * set in napi_schedule_prep().
	 * Since we are about to call napi->poll() once more, we can safely
	 * clear NAPI_STATE_MISSED.
	 *
	 * Note: x86 could use a single "lock and ..." instruction
	 * to perform these two clear_bit()
	 */
	clear_bit(NAPI_STATE_MISSED, &napi->state);
	clear_bit(NAPI_STATE_IN_BUSY_POLL, &napi->state);

	local_bh_disable();

	/* All we really want here is to re-enable device interrupts.
	 * Ideally, a new ndo_busy_poll_stop() could avoid another round.
	 */
	rc = napi->poll(napi, BUSY_POLL_BUDGET);
	trace_napi_poll(napi, rc, BUSY_POLL_BUDGET);
	netpoll_poll_unlock(have_poll_lock);
	if (rc == BUSY_POLL_BUDGET)
		__napi_schedule(napi);
	local_bh_enable();