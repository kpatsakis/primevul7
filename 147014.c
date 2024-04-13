static void find_jobs(int vtime, cron_db * db, int doWild, int doNonWild, long vGMToff) {
	char *orig_tz, *job_tz;
	time_t virtualSecond = vtime * SECONDS_PER_MINUTE;
	time_t virtualGMTSecond = virtualSecond - vGMToff;
	struct tm *tm;
	int minute, hour, dom, month, dow;
	user *u;
	entry *e;
	const char *uname;

	/* The support for the job-specific timezones is not perfect. There will
	 * be jobs missed or run twice during the DST change in the job timezone.
	 * It is recommended not to schedule any jobs during the hour when
	 * the DST changes happen if job-specific timezones are used.
	 *
	 * Make 0-based values out of tm values so we can use them as indicies
	 */
#define maketime(tz1, tz2) do { \
	char *t = tz1; \
	if (t != NULL && *t != '\0') { \
		setenv("TZ", t, 1); \
		tm = localtime(&virtualGMTSecond); \
	} else { if ((tz2) != NULL) \
			setenv("TZ", (tz2), 1); \
		else \
			unsetenv("TZ"); \
		tm = gmtime(&virtualSecond); \
	} \
	minute = tm->tm_min -FIRST_MINUTE; \
	hour = tm->tm_hour -FIRST_HOUR; \
	dom = tm->tm_mday -FIRST_DOM; \
	month = tm->tm_mon +1 /* 0..11 -> 1..12 */ -FIRST_MONTH; \
	dow = tm->tm_wday -FIRST_DOW; \
	} while (0)

	orig_tz = getenv("TZ");
	maketime(NULL, orig_tz);

	Debug(DSCH, ("[%ld] tick(%d,%d,%d,%d,%d) %s %s\n",
			(long) getpid(), minute, hour, dom, month, dow,
			doWild ? " " : "No wildcard", doNonWild ? " " : "Wildcard only"))
		/* the dom/dow situation is odd.  '* * 1,15 * Sun' will run on the
		 * first and fifteenth AND every Sunday;  '* * * * Sun' will run *only*
		 * on Sundays;  '* * 1,15 * *' will run *only* the 1st and 15th.  this
		 * is why we keep 'e->dow_star' and 'e->dom_star'.  yes, it's bizarre.
		 * like many bizarre things, it's the standard.
		 */
		for (u = db->head; u != NULL; u = u->next) {
		for (e = u->crontab; e != NULL; e = e->next) {
			Debug(DSCH | DEXT, ("user [%s:%ld:%ld:...] cmd=\"%s\"\n",
					e->pwd->pw_name, (long) e->pwd->pw_uid,
					(long) e->pwd->pw_gid, e->cmd))
				uname = e->pwd->pw_name;
			/* check if user exists in time of job is being run f.e. ldap */
			if (getpwnam(uname) != NULL) {
				job_tz = env_get("CRON_TZ", e->envp);
				maketime(job_tz, orig_tz);
				/* here we test whether time is NOW */
				if (bit_test(e->minute, minute) &&
					bit_test(e->hour, hour) &&
					bit_test(e->month, month) &&
					(((e->flags & DOM_STAR) || (e->flags & DOW_STAR))
						? (bit_test(e->dow, dow) && bit_test(e->dom, dom))
						: (bit_test(e->dow, dow) || bit_test(e->dom, dom))
					)
					) {
					if (job_tz != NULL && vGMToff != GMToff)
						/* do not try to run the jobs from different timezones
						 * during the DST switch of the default timezone.
						 */
						continue;

					if ((doNonWild &&
							!(e->flags & (MIN_STAR | HR_STAR))) ||
						(doWild && (e->flags & (MIN_STAR | HR_STAR))))
						job_add(e, u);	/*will add job, if it isn't in queue already for NOW. */
				}
			}
		}
	}
	if (orig_tz != NULL)
		setenv("TZ", orig_tz, 1);
	else
		unsetenv("TZ");
}