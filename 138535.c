static void arcmsr_hbaC_doorbell_isr(struct AdapterControlBlock *pACB)
{
	uint32_t outbound_doorbell;
	struct MessageUnit_C __iomem *reg = pACB->pmuC;
	/*
	*******************************************************************
	**  Maybe here we need to check wrqbuffer_lock is lock or not
	**  DOORBELL: din! don!
	**  check if there are any mail need to pack from firmware
	*******************************************************************
	*/
	outbound_doorbell = readl(&reg->outbound_doorbell);
	do {
		writel(outbound_doorbell, &reg->outbound_doorbell_clear);
		readl(&reg->outbound_doorbell_clear);
		if (outbound_doorbell & ARCMSR_HBCMU_IOP2DRV_DATA_WRITE_OK)
			arcmsr_iop2drv_data_wrote_handle(pACB);
		if (outbound_doorbell & ARCMSR_HBCMU_IOP2DRV_DATA_READ_OK)
			arcmsr_iop2drv_data_read_handle(pACB);
		if (outbound_doorbell & ARCMSR_HBCMU_IOP2DRV_MESSAGE_CMD_DONE)
			arcmsr_hbaC_message_isr(pACB);
		outbound_doorbell = readl(&reg->outbound_doorbell);
	} while (outbound_doorbell & (ARCMSR_HBCMU_IOP2DRV_DATA_WRITE_OK
		| ARCMSR_HBCMU_IOP2DRV_DATA_READ_OK
		| ARCMSR_HBCMU_IOP2DRV_MESSAGE_CMD_DONE));
}