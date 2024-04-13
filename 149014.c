drain_zmbuf_list(struct vhost_virtqueue *vq)
{
	struct zcopy_mbuf *zmbuf, *next;

	for (zmbuf = TAILQ_FIRST(&vq->zmbuf_list);
	     zmbuf != NULL; zmbuf = next) {
		next = TAILQ_NEXT(zmbuf, next);

		while (!mbuf_is_consumed(zmbuf->mbuf))
			usleep(1000);

		TAILQ_REMOVE(&vq->zmbuf_list, zmbuf, next);
		restore_mbuf(zmbuf->mbuf);
		rte_pktmbuf_free(zmbuf->mbuf);
		put_zmbuf(zmbuf);
		vq->nr_zmbuf -= 1;
	}
}