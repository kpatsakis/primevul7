numa_realloc(struct virtio_net *dev, int index)
{
	int oldnode, newnode;
	struct virtio_net *old_dev;
	struct vhost_virtqueue *old_vq, *vq;
	struct zcopy_mbuf *new_zmbuf;
	struct vring_used_elem *new_shadow_used_split;
	struct vring_used_elem_packed *new_shadow_used_packed;
	struct batch_copy_elem *new_batch_copy_elems;
	int ret;

	if (dev->flags & VIRTIO_DEV_RUNNING)
		return dev;

	old_dev = dev;
	vq = old_vq = dev->virtqueue[index];

	ret = get_mempolicy(&newnode, NULL, 0, old_vq->desc,
			    MPOL_F_NODE | MPOL_F_ADDR);

	/* check if we need to reallocate vq */
	ret |= get_mempolicy(&oldnode, NULL, 0, old_vq,
			     MPOL_F_NODE | MPOL_F_ADDR);
	if (ret) {
		VHOST_LOG_CONFIG(ERR,
			"Unable to get vq numa information.\n");
		return dev;
	}
	if (oldnode != newnode) {
		VHOST_LOG_CONFIG(INFO,
			"reallocate vq from %d to %d node\n", oldnode, newnode);
		vq = rte_malloc_socket(NULL, sizeof(*vq), 0, newnode);
		if (!vq)
			return dev;

		memcpy(vq, old_vq, sizeof(*vq));
		TAILQ_INIT(&vq->zmbuf_list);

		if (dev->dequeue_zero_copy) {
			new_zmbuf = rte_malloc_socket(NULL, vq->zmbuf_size *
					sizeof(struct zcopy_mbuf), 0, newnode);
			if (new_zmbuf) {
				rte_free(vq->zmbufs);
				vq->zmbufs = new_zmbuf;
			}
		}

		if (vq_is_packed(dev)) {
			new_shadow_used_packed = rte_malloc_socket(NULL,
					vq->size *
					sizeof(struct vring_used_elem_packed),
					RTE_CACHE_LINE_SIZE,
					newnode);
			if (new_shadow_used_packed) {
				rte_free(vq->shadow_used_packed);
				vq->shadow_used_packed = new_shadow_used_packed;
			}
		} else {
			new_shadow_used_split = rte_malloc_socket(NULL,
					vq->size *
					sizeof(struct vring_used_elem),
					RTE_CACHE_LINE_SIZE,
					newnode);
			if (new_shadow_used_split) {
				rte_free(vq->shadow_used_split);
				vq->shadow_used_split = new_shadow_used_split;
			}
		}

		new_batch_copy_elems = rte_malloc_socket(NULL,
			vq->size * sizeof(struct batch_copy_elem),
			RTE_CACHE_LINE_SIZE,
			newnode);
		if (new_batch_copy_elems) {
			rte_free(vq->batch_copy_elems);
			vq->batch_copy_elems = new_batch_copy_elems;
		}

		rte_free(old_vq);
	}

	/* check if we need to reallocate dev */
	ret = get_mempolicy(&oldnode, NULL, 0, old_dev,
			    MPOL_F_NODE | MPOL_F_ADDR);
	if (ret) {
		VHOST_LOG_CONFIG(ERR,
			"Unable to get dev numa information.\n");
		goto out;
	}
	if (oldnode != newnode) {
		VHOST_LOG_CONFIG(INFO,
			"reallocate dev from %d to %d node\n",
			oldnode, newnode);
		dev = rte_malloc_socket(NULL, sizeof(*dev), 0, newnode);
		if (!dev) {
			dev = old_dev;
			goto out;
		}

		memcpy(dev, old_dev, sizeof(*dev));
		rte_free(old_dev);
	}

out:
	dev->virtqueue[index] = vq;
	vhost_devices[dev->vid] = dev;

	if (old_vq != vq)
		vhost_user_iotlb_init(dev, index);

	return dev;
}