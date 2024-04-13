gadgetfs_make_inode (struct super_block *sb,
		void *data, const struct file_operations *fops,
		int mode)
{
	struct inode *inode = new_inode (sb);

	if (inode) {
		inode->i_ino = get_next_ino();
		inode->i_mode = mode;
		inode->i_uid = make_kuid(&init_user_ns, default_uid);
		inode->i_gid = make_kgid(&init_user_ns, default_gid);
		inode->i_atime = inode->i_mtime = inode->i_ctime
				= current_time(inode);
		inode->i_private = data;
		inode->i_fop = fops;
	}
	return inode;
}