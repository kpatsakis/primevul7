static bool ext4_should_use_dax(struct inode *inode)
{
	if (!test_opt(inode->i_sb, DAX))
		return false;
	if (!S_ISREG(inode->i_mode))
		return false;
	if (ext4_should_journal_data(inode))
		return false;
	if (ext4_has_inline_data(inode))
		return false;
	if (ext4_encrypted_inode(inode))
		return false;
	return true;
}