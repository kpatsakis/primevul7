grub_ext4_find_leaf (struct grub_ext2_data *data,
                     struct grub_ext4_extent_header *ext_block,
                     grub_uint32_t fileblock)
{
  struct grub_ext4_extent_idx *index;
  void *buf = NULL;

  while (1)
    {
      int i;
      grub_disk_addr_t block;

      index = (struct grub_ext4_extent_idx *) (ext_block + 1);

      if (ext_block->magic != grub_cpu_to_le16_compile_time (EXT4_EXT_MAGIC))
	goto fail;

      if (ext_block->depth == 0)
        return ext_block;

      for (i = 0; i < grub_le_to_cpu16 (ext_block->entries); i++)
        {
          if (fileblock < grub_le_to_cpu32(index[i].block))
            break;
        }

      if (--i < 0)
	goto fail;

      block = grub_le_to_cpu16 (index[i].leaf_hi);
      block = (block << 32) | grub_le_to_cpu32 (index[i].leaf);
      if (!buf)
	buf = grub_malloc (EXT2_BLOCK_SIZE(data));
      if (!buf)
	goto fail;
      if (grub_disk_read (data->disk,
                          block << LOG2_EXT2_BLOCK_SIZE (data),
                          0, EXT2_BLOCK_SIZE(data), buf))
	goto fail;

      ext_block = buf;
    }
 fail:
  grub_free (buf);
  return 0;
}