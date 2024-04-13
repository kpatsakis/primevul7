grub_ext2_read_block (grub_fshelp_node_t node, grub_disk_addr_t fileblock)
{
  struct grub_ext2_data *data = node->data;
  struct grub_ext2_inode *inode = &node->inode;
  grub_disk_addr_t blknr = -1;
  unsigned int blksz = EXT2_BLOCK_SIZE (data);
  int log2_blksz = LOG2_EXT2_BLOCK_SIZE (data);

  if (inode->flags & grub_cpu_to_le32_compile_time (EXT4_EXTENTS_FLAG))
    {
      struct grub_ext4_extent_header *leaf;
      struct grub_ext4_extent *ext;
      int i;
      grub_disk_addr_t ret;

      leaf = grub_ext4_find_leaf (data, (struct grub_ext4_extent_header *) inode->blocks.dir_blocks, fileblock);
      if (! leaf)
        {
          grub_error (GRUB_ERR_BAD_FS, "invalid extent");
          return -1;
        }

      ext = (struct grub_ext4_extent *) (leaf + 1);
      for (i = 0; i < grub_le_to_cpu16 (leaf->entries); i++)
        {
          if (fileblock < grub_le_to_cpu32 (ext[i].block))
            break;
        }

      if (--i >= 0)
        {
          fileblock -= grub_le_to_cpu32 (ext[i].block);
          if (fileblock >= grub_le_to_cpu16 (ext[i].len))
	    ret = 0;
          else
            {
              grub_disk_addr_t start;

              start = grub_le_to_cpu16 (ext[i].start_hi);
              start = (start << 32) + grub_le_to_cpu32 (ext[i].start);

              ret = fileblock + start;
            }
        }
      else
        {
          grub_error (GRUB_ERR_BAD_FS, "something wrong with extent");
	  ret = -1;
        }

      if (leaf != (struct grub_ext4_extent_header *) inode->blocks.dir_blocks)
	grub_free (leaf);

      return ret;
    }
  /* Direct blocks.  */
  if (fileblock < INDIRECT_BLOCKS)
    blknr = grub_le_to_cpu32 (inode->blocks.dir_blocks[fileblock]);
  /* Indirect.  */
  else if (fileblock < INDIRECT_BLOCKS + blksz / 4)
    {
      grub_uint32_t indir;

      if (grub_disk_read (data->disk,
			  ((grub_disk_addr_t)
			   grub_le_to_cpu32 (inode->blocks.indir_block))
			  << log2_blksz,
			  (fileblock - INDIRECT_BLOCKS) * sizeof (indir),
			  sizeof (indir), &indir))
	return grub_errno;

      blknr = grub_le_to_cpu32 (indir);
    }
  /* Double indirect.  */
  else if (fileblock < INDIRECT_BLOCKS
	   + blksz / 4 * ((grub_disk_addr_t) blksz / 4 + 1))
    {
      int log_perblock = log2_blksz + 9 - 2;
      grub_disk_addr_t rblock = fileblock - (INDIRECT_BLOCKS
					     + blksz / 4);
      grub_uint32_t indir;

      if (grub_disk_read (data->disk,
			  ((grub_disk_addr_t)
			   grub_le_to_cpu32 (inode->blocks.double_indir_block))
			  << log2_blksz,
			  (rblock >> log_perblock) * sizeof (indir),
			  sizeof (indir), &indir))
	return grub_errno;

      if (grub_disk_read (data->disk,
			  ((grub_disk_addr_t)
			   grub_le_to_cpu32 (indir))
			  << log2_blksz,
			  (rblock & ((1 << log_perblock) - 1)) * sizeof (indir),
			  sizeof (indir), &indir))
	return grub_errno;


      blknr = grub_le_to_cpu32 (indir);
    }
  /* triple indirect.  */
  else if (fileblock < INDIRECT_BLOCKS + blksz / 4 * ((grub_disk_addr_t) blksz / 4 + 1)
	   + ((grub_disk_addr_t) blksz / 4) * ((grub_disk_addr_t) blksz / 4)
	   * ((grub_disk_addr_t) blksz / 4 + 1))
    {
      int log_perblock = log2_blksz + 9 - 2;
      grub_disk_addr_t rblock = fileblock - (INDIRECT_BLOCKS + blksz / 4
					     * (blksz / 4 + 1));
      grub_uint32_t indir;

      if (grub_disk_read (data->disk,
			  ((grub_disk_addr_t)
			   grub_le_to_cpu32 (inode->blocks.triple_indir_block))
			  << log2_blksz,
			  ((rblock >> log_perblock) >> log_perblock)
			  * sizeof (indir), sizeof (indir), &indir))
	return grub_errno;

      if (grub_disk_read (data->disk,
			  ((grub_disk_addr_t)
			   grub_le_to_cpu32 (indir))
			  << log2_blksz,
			  ((rblock >> log_perblock)
			   & ((1 << log_perblock) - 1)) * sizeof (indir),
			  sizeof (indir), &indir))
	return grub_errno;

      if (grub_disk_read (data->disk,
			  ((grub_disk_addr_t)
			   grub_le_to_cpu32 (indir))
			  << log2_blksz,
			  (rblock  & ((1 << log_perblock) - 1))
			  * sizeof (indir), sizeof (indir), &indir))
	return grub_errno;

      blknr = grub_le_to_cpu32 (indir);
    }
  else
    {
      grub_error (GRUB_ERR_BAD_FS,
		  "ext2fs doesn't support quadruple indirect blocks");
    }

  return blknr;
}