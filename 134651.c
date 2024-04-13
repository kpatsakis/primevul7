grub_ext2_read_inode (struct grub_ext2_data *data,
		      int ino, struct grub_ext2_inode *inode)
{
  struct grub_ext2_block_group blkgrp;
  struct grub_ext2_sblock *sblock = &data->sblock;
  int inodes_per_block;
  unsigned int blkno;
  unsigned int blkoff;
  grub_disk_addr_t base;

  /* It is easier to calculate if the first inode is 0.  */
  ino--;

  grub_ext2_blockgroup (data,
                        ino / grub_le_to_cpu32 (sblock->inodes_per_group),
			&blkgrp);
  if (grub_errno)
    return grub_errno;

  inodes_per_block = EXT2_BLOCK_SIZE (data) / EXT2_INODE_SIZE (data);
  blkno = (ino % grub_le_to_cpu32 (sblock->inodes_per_group))
    / inodes_per_block;
  blkoff = (ino % grub_le_to_cpu32 (sblock->inodes_per_group))
    % inodes_per_block;

  base = grub_le_to_cpu32 (blkgrp.inode_table_id);
  if (data->log_group_desc_size >= 6)
    base |= (((grub_disk_addr_t) grub_le_to_cpu32 (blkgrp.inode_table_id_hi))
	     << 32);

  /* Read the inode.  */
  if (grub_disk_read (data->disk,
		      ((base + blkno) << LOG2_EXT2_BLOCK_SIZE (data)),
		      EXT2_INODE_SIZE (data) * blkoff,
		      sizeof (struct grub_ext2_inode), inode))
    return grub_errno;

  return 0;
}