static struct aspeed_lpc_ctrl *file_aspeed_lpc_ctrl(struct file *file)
{
	return container_of(file->private_data, struct aspeed_lpc_ctrl,
			miscdev);
}