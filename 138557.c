static int arcmsr_adjust_disk_queue_depth(struct scsi_device *sdev, int queue_depth)
{
	if (queue_depth > ARCMSR_MAX_CMD_PERLUN)
		queue_depth = ARCMSR_MAX_CMD_PERLUN;
	return scsi_change_queue_depth(sdev, queue_depth);
}