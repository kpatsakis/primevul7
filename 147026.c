static void cron_sleep(int target, cron_db * db) {
	time_t t1, t2;
	int seconds_to_wait;

	t1 = time(NULL) + GMToff;
	seconds_to_wait = (int) (target * SECONDS_PER_MINUTE - t1) + 1;
	Debug(DSCH, ("[%ld] Target time=%ld, sec-to-wait=%d\n",
			(long) getpid(), (long) target * SECONDS_PER_MINUTE,
			seconds_to_wait))

		while (seconds_to_wait > 0 && seconds_to_wait < 65) {
		sleep((unsigned int) seconds_to_wait);

		/*
		 * Check to see if we were interrupted by a signal.
		 * If so, service the signal(s) then continue sleeping
		 * where we left off.
		 */
		handle_signals(db);

		t2 = time(NULL) + GMToff;
		seconds_to_wait -= (int) (t2 - t1);
		t1 = t2;
	}
}