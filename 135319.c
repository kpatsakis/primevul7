static int sof_debug_ipc_flood_test(struct snd_sof_dev *sdev,
				    struct snd_sof_dfsentry *dfse,
				    bool flood_duration_test,
				    unsigned long ipc_duration_ms,
				    unsigned long ipc_count)
{
	struct sof_ipc_cmd_hdr hdr;
	struct sof_ipc_reply reply;
	u64 min_response_time = U64_MAX;
	ktime_t start, end, test_end;
	u64 avg_response_time = 0;
	u64 max_response_time = 0;
	u64 ipc_response_time;
	int i = 0;
	int ret;

	/* configure test IPC */
	hdr.cmd = SOF_IPC_GLB_TEST_MSG | SOF_IPC_TEST_IPC_FLOOD;
	hdr.size = sizeof(hdr);

	/* set test end time for duration flood test */
	if (flood_duration_test)
		test_end = ktime_get_ns() + ipc_duration_ms * NSEC_PER_MSEC;

	/* send test IPC's */
	while (1) {
		start = ktime_get();
		ret = sof_ipc_tx_message(sdev->ipc, hdr.cmd, &hdr, hdr.size,
					 &reply, sizeof(reply));
		end = ktime_get();

		if (ret < 0)
			break;

		/* compute min and max response times */
		ipc_response_time = ktime_to_ns(ktime_sub(end, start));
		min_response_time = min(min_response_time, ipc_response_time);
		max_response_time = max(max_response_time, ipc_response_time);

		/* sum up response times */
		avg_response_time += ipc_response_time;
		i++;

		/* test complete? */
		if (flood_duration_test) {
			if (ktime_to_ns(end) >= test_end)
				break;
		} else {
			if (i == ipc_count)
				break;
		}
	}

	if (ret < 0)
		dev_err(sdev->dev,
			"error: ipc flood test failed at %d iterations\n", i);

	/* return if the first IPC fails */
	if (!i)
		return ret;

	/* compute average response time */
	do_div(avg_response_time, i);

	/* clear previous test output */
	memset(dfse->cache_buf, 0, IPC_FLOOD_TEST_RESULT_LEN);

	if (flood_duration_test) {
		dev_dbg(sdev->dev, "IPC Flood test duration: %lums\n",
			ipc_duration_ms);
		snprintf(dfse->cache_buf, IPC_FLOOD_TEST_RESULT_LEN,
			 "IPC Flood test duration: %lums\n", ipc_duration_ms);
	}

	dev_dbg(sdev->dev,
		"IPC Flood count: %d, Avg response time: %lluns\n",
		i, avg_response_time);
	dev_dbg(sdev->dev, "Max response time: %lluns\n",
		max_response_time);
	dev_dbg(sdev->dev, "Min response time: %lluns\n",
		min_response_time);

	/* format output string */
	snprintf(dfse->cache_buf + strlen(dfse->cache_buf),
		 IPC_FLOOD_TEST_RESULT_LEN - strlen(dfse->cache_buf),
		 "IPC Flood count: %d\nAvg response time: %lluns\n",
		 i, avg_response_time);

	snprintf(dfse->cache_buf + strlen(dfse->cache_buf),
		 IPC_FLOOD_TEST_RESULT_LEN - strlen(dfse->cache_buf),
		 "Max response time: %lluns\nMin response time: %lluns\n",
		 max_response_time, min_response_time);

	return ret;
}