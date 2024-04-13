free_mem_region(struct virtio_net *dev)
{
	uint32_t i;
	struct rte_vhost_mem_region *reg;
	struct vhost_virtqueue *vq;

	if (!dev || !dev->mem)
		return;

	if (dev->dequeue_zero_copy) {
		for (i = 0; i < dev->nr_vring; i++) {
			vq = dev->virtqueue[i];
			if (vq)
				drain_zmbuf_list(vq);
		}
	}

	for (i = 0; i < dev->mem->nregions; i++) {
		reg = &dev->mem->regions[i];
		if (reg->host_user_addr) {
			munmap(reg->mmap_addr, reg->mmap_size);
			close(reg->fd);
		}
	}
}