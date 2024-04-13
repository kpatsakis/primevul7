get_pervq_shm_size_split(uint16_t queue_size)
{
	return RTE_ALIGN_MUL_CEIL(sizeof(struct rte_vhost_inflight_desc_split) *
				  queue_size + sizeof(uint64_t) +
				  sizeof(uint16_t) * 4, INFLIGHT_ALIGNMENT);
}