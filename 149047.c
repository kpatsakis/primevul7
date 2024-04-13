static int is_vring_iotlb(struct virtio_net *dev,
			  struct vhost_virtqueue *vq,
			  struct vhost_iotlb_msg *imsg)
{
	if (vq_is_packed(dev))
		return is_vring_iotlb_packed(vq, imsg);
	else
		return is_vring_iotlb_split(vq, imsg);
}