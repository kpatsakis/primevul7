static void arcmsr_hbaD_doorbell_isr(struct AdapterControlBlock *pACB)
{
	uint32_t outbound_doorbell;
	struct MessageUnit_D  *pmu = pACB->pmuD;

	outbound_doorbell = readl(pmu->outbound_doorbell);
	do {
		writel(outbound_doorbell, pmu->outbound_doorbell);
		if (outbound_doorbell & ARCMSR_ARC1214_IOP2DRV_MESSAGE_CMD_DONE)
			arcmsr_hbaD_message_isr(pACB);
		if (outbound_doorbell & ARCMSR_ARC1214_IOP2DRV_DATA_WRITE_OK)
			arcmsr_iop2drv_data_wrote_handle(pACB);
		if (outbound_doorbell & ARCMSR_ARC1214_IOP2DRV_DATA_READ_OK)
			arcmsr_iop2drv_data_read_handle(pACB);
		outbound_doorbell = readl(pmu->outbound_doorbell);
	} while (outbound_doorbell & (ARCMSR_ARC1214_IOP2DRV_DATA_WRITE_OK
		| ARCMSR_ARC1214_IOP2DRV_DATA_READ_OK
		| ARCMSR_ARC1214_IOP2DRV_MESSAGE_CMD_DONE));
}