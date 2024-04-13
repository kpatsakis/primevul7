format_absolute_time(uint64_t t, char *buf, size_t len)
{
	time_t tt = t > SSH_TIME_T_MAX ? SSH_TIME_T_MAX : t;
	struct tm tm;

	localtime_r(&tt, &tm);
	strftime(buf, len, "%Y-%m-%dT%H:%M:%S", &tm);
}