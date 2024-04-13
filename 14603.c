monotime_double(void)
{
	struct timespec ts;

	monotime_ts(&ts);
	return ts.tv_sec + ((double)ts.tv_nsec / 1000000000);
}