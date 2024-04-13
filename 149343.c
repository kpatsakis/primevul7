static int ccp_init_sg_workarea(struct ccp_sg_workarea *wa, struct device *dev,
				struct scatterlist *sg, u64 len,
				enum dma_data_direction dma_dir)
{
	memset(wa, 0, sizeof(*wa));

	wa->sg = sg;
	if (!sg)
		return 0;

	wa->nents = sg_nents_for_len(sg, len);
	if (wa->nents < 0)
		return wa->nents;

	wa->bytes_left = len;
	wa->sg_used = 0;

	if (len == 0)
		return 0;

	if (dma_dir == DMA_NONE)
		return 0;

	wa->dma_sg = sg;
	wa->dma_dev = dev;
	wa->dma_dir = dma_dir;
	wa->dma_count = dma_map_sg(dev, sg, wa->nents, dma_dir);
	if (!wa->dma_count)
		return -ENOMEM;

	return 0;
}