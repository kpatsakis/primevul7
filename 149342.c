static void ccp_update_sg_workarea(struct ccp_sg_workarea *wa, unsigned int len)
{
	unsigned int nbytes = min_t(u64, len, wa->bytes_left);

	if (!wa->sg)
		return;

	wa->sg_used += nbytes;
	wa->bytes_left -= nbytes;
	if (wa->sg_used == wa->sg->length) {
		wa->sg = sg_next(wa->sg);
		wa->sg_used = 0;
	}
}