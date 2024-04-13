static void sas_chain_work(struct sas_ha_struct *ha, struct sas_work *sw)
{
	/* chained work is not subject to SA_HA_DRAINING or
	 * SAS_HA_REGISTERED, because it is either submitted in the
	 * workqueue, or known to be submitted from a context that is
	 * not racing against draining
	 */
	queue_work(ha->disco_q, &sw->work);
}