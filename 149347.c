ccp_run_des3_cmd(struct ccp_cmd_queue *cmd_q, struct ccp_cmd *cmd)
{
	struct ccp_des3_engine *des3 = &cmd->u.des3;

	struct ccp_dm_workarea key, ctx;
	struct ccp_data src, dst;
	struct ccp_op op;
	unsigned int dm_offset;
	unsigned int len_singlekey;
	bool in_place = false;
	int ret;

	/* Error checks */
	if (cmd_q->ccp->vdata->version < CCP_VERSION(5, 0))
		return -EINVAL;

	if (!cmd_q->ccp->vdata->perform->des3)
		return -EINVAL;

	if (des3->key_len != DES3_EDE_KEY_SIZE)
		return -EINVAL;

	if (((des3->mode == CCP_DES3_MODE_ECB) ||
		(des3->mode == CCP_DES3_MODE_CBC)) &&
		(des3->src_len & (DES3_EDE_BLOCK_SIZE - 1)))
		return -EINVAL;

	if (!des3->key || !des3->src || !des3->dst)
		return -EINVAL;

	if (des3->mode != CCP_DES3_MODE_ECB) {
		if (des3->iv_len != DES3_EDE_BLOCK_SIZE)
			return -EINVAL;

		if (!des3->iv)
			return -EINVAL;
	}

	ret = -EIO;
	/* Zero out all the fields of the command desc */
	memset(&op, 0, sizeof(op));

	/* Set up the Function field */
	op.cmd_q = cmd_q;
	op.jobid = CCP_NEW_JOBID(cmd_q->ccp);
	op.sb_key = cmd_q->sb_key;

	op.init = (des3->mode == CCP_DES3_MODE_ECB) ? 0 : 1;
	op.u.des3.type = des3->type;
	op.u.des3.mode = des3->mode;
	op.u.des3.action = des3->action;

	/*
	 * All supported key sizes fit in a single (32-byte) KSB entry and
	 * (like AES) must be in little endian format. Use the 256-bit byte
	 * swap passthru option to convert from big endian to little endian.
	 */
	ret = ccp_init_dm_workarea(&key, cmd_q,
				   CCP_DES3_KEY_SB_COUNT * CCP_SB_BYTES,
				   DMA_TO_DEVICE);
	if (ret)
		return ret;

	/*
	 * The contents of the key triplet are in the reverse order of what
	 * is required by the engine. Copy the 3 pieces individually to put
	 * them where they belong.
	 */
	dm_offset = CCP_SB_BYTES - des3->key_len; /* Basic offset */

	len_singlekey = des3->key_len / 3;
	ret = ccp_set_dm_area(&key, dm_offset + 2 * len_singlekey,
			      des3->key, 0, len_singlekey);
	if (ret)
		goto e_key;
	ret = ccp_set_dm_area(&key, dm_offset + len_singlekey,
			      des3->key, len_singlekey, len_singlekey);
	if (ret)
		goto e_key;
	ret = ccp_set_dm_area(&key, dm_offset,
			      des3->key, 2 * len_singlekey, len_singlekey);
	if (ret)
		goto e_key;

	/* Copy the key to the SB */
	ret = ccp_copy_to_sb(cmd_q, &key, op.jobid, op.sb_key,
			     CCP_PASSTHRU_BYTESWAP_256BIT);
	if (ret) {
		cmd->engine_error = cmd_q->cmd_error;
		goto e_key;
	}

	/*
	 * The DES3 context fits in a single (32-byte) KSB entry and
	 * must be in little endian format. Use the 256-bit byte swap
	 * passthru option to convert from big endian to little endian.
	 */
	if (des3->mode != CCP_DES3_MODE_ECB) {
		op.sb_ctx = cmd_q->sb_ctx;

		ret = ccp_init_dm_workarea(&ctx, cmd_q,
					   CCP_DES3_CTX_SB_COUNT * CCP_SB_BYTES,
					   DMA_BIDIRECTIONAL);
		if (ret)
			goto e_key;

		/* Load the context into the LSB */
		dm_offset = CCP_SB_BYTES - des3->iv_len;
		ret = ccp_set_dm_area(&ctx, dm_offset, des3->iv, 0,
				      des3->iv_len);
		if (ret)
			goto e_ctx;

		ret = ccp_copy_to_sb(cmd_q, &ctx, op.jobid, op.sb_ctx,
				     CCP_PASSTHRU_BYTESWAP_256BIT);
		if (ret) {
			cmd->engine_error = cmd_q->cmd_error;
			goto e_ctx;
		}
	}

	/*
	 * Prepare the input and output data workareas. For in-place
	 * operations we need to set the dma direction to BIDIRECTIONAL
	 * and copy the src workarea to the dst workarea.
	 */
	if (sg_virt(des3->src) == sg_virt(des3->dst))
		in_place = true;

	ret = ccp_init_data(&src, cmd_q, des3->src, des3->src_len,
			DES3_EDE_BLOCK_SIZE,
			in_place ? DMA_BIDIRECTIONAL : DMA_TO_DEVICE);
	if (ret)
		goto e_ctx;

	if (in_place)
		dst = src;
	else {
		ret = ccp_init_data(&dst, cmd_q, des3->dst, des3->src_len,
				DES3_EDE_BLOCK_SIZE, DMA_FROM_DEVICE);
		if (ret)
			goto e_src;
	}

	/* Send data to the CCP DES3 engine */
	while (src.sg_wa.bytes_left) {
		ccp_prepare_data(&src, &dst, &op, DES3_EDE_BLOCK_SIZE, true);
		if (!src.sg_wa.bytes_left) {
			op.eom = 1;

			/* Since we don't retrieve the context in ECB mode
			 * we have to wait for the operation to complete
			 * on the last piece of data
			 */
			op.soc = 0;
		}

		ret = cmd_q->ccp->vdata->perform->des3(&op);
		if (ret) {
			cmd->engine_error = cmd_q->cmd_error;
			goto e_dst;
		}

		ccp_process_data(&src, &dst, &op);
	}

	if (des3->mode != CCP_DES3_MODE_ECB) {
		/* Retrieve the context and make BE */
		ret = ccp_copy_from_sb(cmd_q, &ctx, op.jobid, op.sb_ctx,
				       CCP_PASSTHRU_BYTESWAP_256BIT);
		if (ret) {
			cmd->engine_error = cmd_q->cmd_error;
			goto e_dst;
		}

		/* ...but we only need the last DES3_EDE_BLOCK_SIZE bytes */
		ccp_get_dm_area(&ctx, dm_offset, des3->iv, 0,
				DES3_EDE_BLOCK_SIZE);
	}
e_dst:
	if (!in_place)
		ccp_free_data(&dst, cmd_q);

e_src:
	ccp_free_data(&src, cmd_q);

e_ctx:
	if (des3->mode != CCP_DES3_MODE_ECB)
		ccp_dm_free(&ctx);

e_key:
	ccp_dm_free(&key);

	return ret;
}