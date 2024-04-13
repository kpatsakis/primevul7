get_pervq_shm_size_packed(uint16_t queue_size)
{
	return RTE_ALIGN_MUL_CEIL(sizeof(struct rte_vhost_inflight_desc_packed)
				  * queue_size + sizeof(uint64_t) +
				  sizeof(uint16_t) * 6 + sizeof(uint8_t) * 9,
				  INFLIGHT_ALIGNMENT);
}