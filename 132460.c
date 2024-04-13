int detect_shared_rootfs(void)
{
	if (is_shared_mountpoint("/"))
		return 1;
	return 0;
}