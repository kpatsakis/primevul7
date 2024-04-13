vidtv_s302m_compute_sample_count_from_video(struct vidtv_encoder *e)
{
	struct vidtv_access_unit *sync_au = e->sync->access_units;
	struct vidtv_access_unit *au = e->access_units;
	u32 sample_duration_usecs;
	u32 vau_duration_usecs;
	u32 s;

	vau_duration_usecs    = USEC_PER_SEC / e->sync->sampling_rate_hz;
	sample_duration_usecs = USEC_PER_SEC / e->sampling_rate_hz;

	while (au && sync_au) {
		s = DIV_ROUND_UP(vau_duration_usecs, sample_duration_usecs);
		au->num_samples = s;
		au = au->next;
		sync_au = sync_au->next;
	}
}