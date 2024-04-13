grub_ext2_mount (grub_disk_t disk)
{
  struct grub_ext2_data *data;

  data = grub_malloc (sizeof (struct grub_ext2_data));
  if (!data)
    return 0;

  /* Read the superblock.  */
  grub_disk_read (disk, 1 * 2, 0, sizeof (struct grub_ext2_sblock),
                  &data->sblock);
  if (grub_errno)
    goto fail;

  /* Make sure this is an ext2 filesystem.  */
  if (data->sblock.magic != grub_cpu_to_le16_compile_time (EXT2_MAGIC)
      || grub_le_to_cpu32 (data->sblock.log2_block_size) >= 16)
    {
      grub_error (GRUB_ERR_BAD_FS, "not an ext2 filesystem");
      goto fail;
    }

  /* Check the FS doesn't have feature bits enabled that we don't support. */
  if (data->sblock.revision_level != grub_cpu_to_le32_compile_time (EXT2_GOOD_OLD_REVISION)
      && (data->sblock.feature_incompat
	  & grub_cpu_to_le32_compile_time (~(EXT2_DRIVER_SUPPORTED_INCOMPAT
					     | EXT2_DRIVER_IGNORED_INCOMPAT))))
    {
      grub_error (GRUB_ERR_BAD_FS, "filesystem has unsupported incompatible features");
      goto fail;
    }

  if (data->sblock.revision_level != grub_cpu_to_le32_compile_time (EXT2_GOOD_OLD_REVISION)
      && (data->sblock.feature_incompat
	  & grub_cpu_to_le32_compile_time (EXT4_FEATURE_INCOMPAT_64BIT))
      && data->sblock.group_desc_size != 0
      && ((data->sblock.group_desc_size & (data->sblock.group_desc_size - 1))
	  == 0)
      && (data->sblock.group_desc_size & grub_cpu_to_le16_compile_time (0x1fe0)))
    {
      grub_uint16_t b = grub_le_to_cpu16 (data->sblock.group_desc_size);
      for (data->log_group_desc_size = 0; b != (1 << data->log_group_desc_size);
	   data->log_group_desc_size++);
    }
  else
    data->log_group_desc_size = 5;

  data->disk = disk;

  data->diropen.data = data;
  data->diropen.ino = 2;
  data->diropen.inode_read = 1;

  data->inode = &data->diropen.inode;

  grub_ext2_read_inode (data, 2, data->inode);
  if (grub_errno)
    goto fail;

  return data;

 fail:
  if (grub_errno == GRUB_ERR_OUT_OF_RANGE)
    grub_error (GRUB_ERR_BAD_FS, "not an ext2 filesystem");

  grub_free (data);
  return 0;
}