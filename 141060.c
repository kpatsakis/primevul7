static void selinux_sb_free_security(struct super_block *sb)
{
	superblock_free_security(sb);
}