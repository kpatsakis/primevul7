static bool arcmsr_hbaD_wait_msgint_ready(struct AdapterControlBlock *pACB)
{
	struct MessageUnit_D *reg = pACB->pmuD;
	int i;

	for (i = 0; i < 2000; i++) {
		if (readl(reg->outbound_doorbell)
			& ARCMSR_ARC1214_IOP2DRV_MESSAGE_CMD_DONE) {
			writel(ARCMSR_ARC1214_IOP2DRV_MESSAGE_CMD_DONE,
				reg->outbound_doorbell);
			return true;
		}
		msleep(10);
	} /* max 20 seconds */
	return false;
}