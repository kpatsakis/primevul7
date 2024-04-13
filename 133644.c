 */
int netif_get_num_default_rss_queues(void)
{
	return is_kdump_kernel() ?
		1 : min_t(int, DEFAULT_MAX_NUM_RSS_QUEUES, num_online_cpus());