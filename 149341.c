static int ccp_run_ecc_pm_cmd(struct ccp_cmd_queue *cmd_q, struct ccp_cmd *cmd)
{
	struct ccp_ecc_engine *ecc = &cmd->u.ecc;
	struct ccp_dm_workarea src, dst;
	struct ccp_op op;
	int ret;
	u8 *save;

	if (!ecc->u.pm.point_1.x ||
	    (ecc->u.pm.point_1.x_len > CCP_ECC_MODULUS_BYTES) ||
	    !ecc->u.pm.point_1.y ||
	    (ecc->u.pm.point_1.y_len > CCP_ECC_MODULUS_BYTES))
		return -EINVAL;

	if (ecc->function == CCP_ECC_FUNCTION_PADD_384BIT) {
		if (!ecc->u.pm.point_2.x ||
		    (ecc->u.pm.point_2.x_len > CCP_ECC_MODULUS_BYTES) ||
		    !ecc->u.pm.point_2.y ||
		    (ecc->u.pm.point_2.y_len > CCP_ECC_MODULUS_BYTES))
			return -EINVAL;
	} else {
		if (!ecc->u.pm.domain_a ||
		    (ecc->u.pm.domain_a_len > CCP_ECC_MODULUS_BYTES))
			return -EINVAL;

		if (ecc->function == CCP_ECC_FUNCTION_PMUL_384BIT)
			if (!ecc->u.pm.scalar ||
			    (ecc->u.pm.scalar_len > CCP_ECC_MODULUS_BYTES))
				return -EINVAL;
	}

	if (!ecc->u.pm.result.x ||
	    (ecc->u.pm.result.x_len < CCP_ECC_MODULUS_BYTES) ||
	    !ecc->u.pm.result.y ||
	    (ecc->u.pm.result.y_len < CCP_ECC_MODULUS_BYTES))
		return -EINVAL;

	memset(&op, 0, sizeof(op));
	op.cmd_q = cmd_q;
	op.jobid = CCP_NEW_JOBID(cmd_q->ccp);

	/* Concatenate the modulus and the operands. Both the modulus and
	 * the operands must be in little endian format.  Since the input
	 * is in big endian format it must be converted and placed in a
	 * fixed length buffer.
	 */
	ret = ccp_init_dm_workarea(&src, cmd_q, CCP_ECC_SRC_BUF_SIZE,
				   DMA_TO_DEVICE);
	if (ret)
		return ret;

	/* Save the workarea address since it is updated in order to perform
	 * the concatenation
	 */
	save = src.address;

	/* Copy the ECC modulus */
	ret = ccp_reverse_set_dm_area(&src, 0, ecc->mod, 0, ecc->mod_len);
	if (ret)
		goto e_src;
	src.address += CCP_ECC_OPERAND_SIZE;

	/* Copy the first point X and Y coordinate */
	ret = ccp_reverse_set_dm_area(&src, 0, ecc->u.pm.point_1.x, 0,
				      ecc->u.pm.point_1.x_len);
	if (ret)
		goto e_src;
	src.address += CCP_ECC_OPERAND_SIZE;
	ret = ccp_reverse_set_dm_area(&src, 0, ecc->u.pm.point_1.y, 0,
				      ecc->u.pm.point_1.y_len);
	if (ret)
		goto e_src;
	src.address += CCP_ECC_OPERAND_SIZE;

	/* Set the first point Z coordinate to 1 */
	*src.address = 0x01;
	src.address += CCP_ECC_OPERAND_SIZE;

	if (ecc->function == CCP_ECC_FUNCTION_PADD_384BIT) {
		/* Copy the second point X and Y coordinate */
		ret = ccp_reverse_set_dm_area(&src, 0, ecc->u.pm.point_2.x, 0,
					      ecc->u.pm.point_2.x_len);
		if (ret)
			goto e_src;
		src.address += CCP_ECC_OPERAND_SIZE;
		ret = ccp_reverse_set_dm_area(&src, 0, ecc->u.pm.point_2.y, 0,
					      ecc->u.pm.point_2.y_len);
		if (ret)
			goto e_src;
		src.address += CCP_ECC_OPERAND_SIZE;

		/* Set the second point Z coordinate to 1 */
		*src.address = 0x01;
		src.address += CCP_ECC_OPERAND_SIZE;
	} else {
		/* Copy the Domain "a" parameter */
		ret = ccp_reverse_set_dm_area(&src, 0, ecc->u.pm.domain_a, 0,
					      ecc->u.pm.domain_a_len);
		if (ret)
			goto e_src;
		src.address += CCP_ECC_OPERAND_SIZE;

		if (ecc->function == CCP_ECC_FUNCTION_PMUL_384BIT) {
			/* Copy the scalar value */
			ret = ccp_reverse_set_dm_area(&src, 0,
						      ecc->u.pm.scalar, 0,
						      ecc->u.pm.scalar_len);
			if (ret)
				goto e_src;
			src.address += CCP_ECC_OPERAND_SIZE;
		}
	}

	/* Restore the workarea address */
	src.address = save;

	/* Prepare the output area for the operation */
	ret = ccp_init_dm_workarea(&dst, cmd_q, CCP_ECC_DST_BUF_SIZE,
				   DMA_FROM_DEVICE);
	if (ret)
		goto e_src;

	op.soc = 1;
	op.src.u.dma.address = src.dma.address;
	op.src.u.dma.offset = 0;
	op.src.u.dma.length = src.length;
	op.dst.u.dma.address = dst.dma.address;
	op.dst.u.dma.offset = 0;
	op.dst.u.dma.length = dst.length;

	op.u.ecc.function = cmd->u.ecc.function;

	ret = cmd_q->ccp->vdata->perform->ecc(&op);
	if (ret) {
		cmd->engine_error = cmd_q->cmd_error;
		goto e_dst;
	}

	ecc->ecc_result = le16_to_cpup(
		(const __le16 *)(dst.address + CCP_ECC_RESULT_OFFSET));
	if (!(ecc->ecc_result & CCP_ECC_RESULT_SUCCESS)) {
		ret = -EIO;
		goto e_dst;
	}

	/* Save the workarea address since it is updated as we walk through
	 * to copy the point math result
	 */
	save = dst.address;

	/* Save the ECC result X and Y coordinates */
	ccp_reverse_get_dm_area(&dst, 0, ecc->u.pm.result.x, 0,
				CCP_ECC_MODULUS_BYTES);
	dst.address += CCP_ECC_OUTPUT_SIZE;
	ccp_reverse_get_dm_area(&dst, 0, ecc->u.pm.result.y, 0,
				CCP_ECC_MODULUS_BYTES);
	dst.address += CCP_ECC_OUTPUT_SIZE;

	/* Restore the workarea address */
	dst.address = save;

e_dst:
	ccp_dm_free(&dst);

e_src:
	ccp_dm_free(&src);

	return ret;
}