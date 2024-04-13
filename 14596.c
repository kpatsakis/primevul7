monotime(void)
{
	struct timespec ts;

	monotime_ts(&ts);
	return ts.tv_sec;
}