log_addr_to_gpa(struct virtio_net *dev, struct vhost_virtqueue *vq)
{
	uint64_t log_gpa;

	vhost_user_iotlb_rd_lock(vq);
	log_gpa = translate_log_addr(dev, vq, vq->ring_addrs.log_guest_addr);
	vhost_user_iotlb_rd_unlock(vq);

	return log_gpa;
}