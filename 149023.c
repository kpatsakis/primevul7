free_zmbufs(struct vhost_virtqueue *vq)
{
	drain_zmbuf_list(vq);

	rte_free(vq->zmbufs);
}