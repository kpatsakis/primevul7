grub_ext2_blockgroup (struct grub_ext2_data *data, int group,
		      struct grub_ext2_block_group *blkgrp)
{
  return grub_disk_read (data->disk,
                         ((grub_le_to_cpu32 (data->sblock.first_data_block) + 1)
                          << LOG2_EXT2_BLOCK_SIZE (data)),
			 group << data->log_group_desc_size,
			 sizeof (struct grub_ext2_block_group), blkgrp);
}