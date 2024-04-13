static int arcmsr_suspend(struct pci_dev *pdev, pm_message_t state)
{
	uint32_t intmask_org;
	struct Scsi_Host *host = pci_get_drvdata(pdev);
	struct AdapterControlBlock *acb =
		(struct AdapterControlBlock *)host->hostdata;

	intmask_org = arcmsr_disable_outbound_ints(acb);
	arcmsr_free_irq(pdev, acb);
	del_timer_sync(&acb->eternal_timer);
	flush_work(&acb->arcmsr_do_message_isr_bh);
	arcmsr_stop_adapter_bgrb(acb);
	arcmsr_flush_adapter_cache(acb);
	pci_set_drvdata(pdev, host);
	pci_save_state(pdev);
	pci_disable_device(pdev);
	pci_set_power_state(pdev, pci_choose_state(pdev, state));
	return 0;
}