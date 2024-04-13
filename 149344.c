static void ccp_prepare_data(struct ccp_data *src, struct ccp_data *dst,
			     struct ccp_op *op, unsigned int block_size,
			     bool blocksize_op)
{
	unsigned int sg_src_len, sg_dst_len, op_len;

	/* The CCP can only DMA from/to one address each per operation. This
	 * requires that we find the smallest DMA area between the source
	 * and destination. The resulting len values will always be <= UINT_MAX
	 * because the dma length is an unsigned int.
	 */
	sg_src_len = sg_dma_len(src->sg_wa.sg) - src->sg_wa.sg_used;
	sg_src_len = min_t(u64, src->sg_wa.bytes_left, sg_src_len);

	if (dst) {
		sg_dst_len = sg_dma_len(dst->sg_wa.sg) - dst->sg_wa.sg_used;
		sg_dst_len = min_t(u64, src->sg_wa.bytes_left, sg_dst_len);
		op_len = min(sg_src_len, sg_dst_len);
	} else {
		op_len = sg_src_len;
	}

	/* The data operation length will be at least block_size in length
	 * or the smaller of available sg room remaining for the source or
	 * the destination
	 */
	op_len = max(op_len, block_size);

	/* Unless we have to buffer data, there's no reason to wait */
	op->soc = 0;

	if (sg_src_len < block_size) {
		/* Not enough data in the sg element, so it
		 * needs to be buffered into a blocksize chunk
		 */
		int cp_len = ccp_fill_queue_buf(src);

		op->soc = 1;
		op->src.u.dma.address = src->dm_wa.dma.address;
		op->src.u.dma.offset = 0;
		op->src.u.dma.length = (blocksize_op) ? block_size : cp_len;
	} else {
		/* Enough data in the sg element, but we need to
		 * adjust for any previously copied data
		 */
		op->src.u.dma.address = sg_dma_address(src->sg_wa.sg);
		op->src.u.dma.offset = src->sg_wa.sg_used;
		op->src.u.dma.length = op_len & ~(block_size - 1);

		ccp_update_sg_workarea(&src->sg_wa, op->src.u.dma.length);
	}

	if (dst) {
		if (sg_dst_len < block_size) {
			/* Not enough room in the sg element or we're on the
			 * last piece of data (when using padding), so the
			 * output needs to be buffered into a blocksize chunk
			 */
			op->soc = 1;
			op->dst.u.dma.address = dst->dm_wa.dma.address;
			op->dst.u.dma.offset = 0;
			op->dst.u.dma.length = op->src.u.dma.length;
		} else {
			/* Enough room in the sg element, but we need to
			 * adjust for any previously used area
			 */
			op->dst.u.dma.address = sg_dma_address(dst->sg_wa.sg);
			op->dst.u.dma.offset = dst->sg_wa.sg_used;
			op->dst.u.dma.length = op->src.u.dma.length;
		}
	}
}