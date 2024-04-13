static void delayed_superblock_init(struct super_block *sb, void *unused)
{
	superblock_doinit(sb, NULL);
}