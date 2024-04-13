static unsigned int ccp_queue_buf(struct ccp_data *data, unsigned int from)
{
	struct ccp_sg_workarea *sg_wa = &data->sg_wa;
	struct ccp_dm_workarea *dm_wa = &data->dm_wa;
	unsigned int buf_count, nbytes;

	/* Clear the buffer if setting it */
	if (!from)
		memset(dm_wa->address, 0, dm_wa->length);

	if (!sg_wa->sg)
		return 0;

	/* Perform the copy operation
	 *   nbytes will always be <= UINT_MAX because dm_wa->length is
	 *   an unsigned int
	 */
	nbytes = min_t(u64, sg_wa->bytes_left, dm_wa->length);
	scatterwalk_map_and_copy(dm_wa->address, sg_wa->sg, sg_wa->sg_used,
				 nbytes, from);

	/* Update the structures and generate the count */
	buf_count = 0;
	while (sg_wa->bytes_left && (buf_count < dm_wa->length)) {
		nbytes = min(sg_wa->sg->length - sg_wa->sg_used,
			     dm_wa->length - buf_count);
		nbytes = min_t(u64, sg_wa->bytes_left, nbytes);

		buf_count += nbytes;
		ccp_update_sg_workarea(sg_wa, nbytes);
	}

	return buf_count;
}