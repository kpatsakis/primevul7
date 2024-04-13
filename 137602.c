static long btrfs_ioctl_clone_range(struct file *file, void __user *argp)
{
	struct btrfs_ioctl_clone_range_args args;

	if (copy_from_user(&args, argp, sizeof(args)))
		return -EFAULT;
	return btrfs_ioctl_clone(file, args.src_fd, args.src_offset,
				 args.src_length, args.dest_offset);
}