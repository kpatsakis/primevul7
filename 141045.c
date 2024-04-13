static int selinux_sb_alloc_security(struct super_block *sb)
{
	return superblock_alloc_security(sb);
}