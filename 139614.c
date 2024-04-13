ext4_journalled_write_inline_data(struct inode *inode,
				  unsigned len,
				  struct page *page)
{
	int ret, no_expand;
	void *kaddr;
	struct ext4_iloc iloc;

	ret = ext4_get_inode_loc(inode, &iloc);
	if (ret) {
		ext4_std_error(inode->i_sb, ret);
		return NULL;
	}

	ext4_write_lock_xattr(inode, &no_expand);
	kaddr = kmap_atomic(page);
	ext4_write_inline_data(inode, &iloc, kaddr, 0, len);
	kunmap_atomic(kaddr);
	ext4_write_unlock_xattr(inode, &no_expand);

	return iloc.bh;
}