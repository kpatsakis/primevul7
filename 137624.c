static long btrfs_ioctl_scrub_cancel(struct btrfs_root *root, void __user *arg)
{
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	return btrfs_scrub_cancel(root->fs_info);
}