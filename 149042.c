ring_addr_to_vva(struct virtio_net *dev, struct vhost_virtqueue *vq,
		uint64_t ra, uint64_t *size)
{
	if (dev->features & (1ULL << VIRTIO_F_IOMMU_PLATFORM)) {
		uint64_t vva;

		vhost_user_iotlb_rd_lock(vq);
		vva = vhost_iova_to_vva(dev, vq, ra,
					size, VHOST_ACCESS_RW);
		vhost_user_iotlb_rd_unlock(vq);

		return vva;
	}

	return qva_to_vva(dev, ra, size);
}