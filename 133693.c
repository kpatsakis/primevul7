
static enum hrtimer_restart napi_watchdog(struct hrtimer *timer)
{
	struct napi_struct *napi;

	napi = container_of(timer, struct napi_struct, timer);

	/* Note : we use a relaxed variant of napi_schedule_prep() not setting
	 * NAPI_STATE_MISSED, since we do not react to a device IRQ.
	 */
	if (napi->gro_bitmask && !napi_disable_pending(napi) &&
	    !test_and_set_bit(NAPI_STATE_SCHED, &napi->state))
		__napi_schedule_irqoff(napi);

	return HRTIMER_NORESTART;