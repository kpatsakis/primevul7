static struct file *do_open_execat(int fd, struct filename *name, int flags)
{
	struct file *file;
	int err;
	struct open_flags open_exec_flags = {
		.open_flag = O_LARGEFILE | O_RDONLY | __FMODE_EXEC,
		.acc_mode = MAY_EXEC,
		.intent = LOOKUP_OPEN,
		.lookup_flags = LOOKUP_FOLLOW,
	};

	if ((flags & ~(AT_SYMLINK_NOFOLLOW | AT_EMPTY_PATH)) != 0)
		return ERR_PTR(-EINVAL);
	if (flags & AT_SYMLINK_NOFOLLOW)
		open_exec_flags.lookup_flags &= ~LOOKUP_FOLLOW;
	if (flags & AT_EMPTY_PATH)
		open_exec_flags.lookup_flags |= LOOKUP_EMPTY;

	file = do_filp_open(fd, name, &open_exec_flags);
	if (IS_ERR(file))
		goto out;

	err = -EACCES;
	if (!S_ISREG(file_inode(file)->i_mode))
		goto exit;

	if (path_noexec(&file->f_path))
		goto exit;

	err = deny_write_access(file);
	if (err)
		goto exit;

	if (name->name[0] != '\0')
		fsnotify_open(file);

out:
	return file;

exit:
	fput(file);
	return ERR_PTR(err);
}