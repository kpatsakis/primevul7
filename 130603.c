void sas_notify_lldd_dev_gone(struct domain_device *dev)
{
	struct sas_ha_struct *sas_ha = dev->port->ha;
	struct Scsi_Host *shost = sas_ha->core.shost;
	struct sas_internal *i = to_sas_internal(shost->transportt);

	if (!i->dft->lldd_dev_gone)
		return;

	if (test_and_clear_bit(SAS_DEV_FOUND, &dev->state)) {
		i->dft->lldd_dev_gone(dev);
		sas_put_device(dev);
	}
}