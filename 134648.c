grub_ext2_read_file (grub_fshelp_node_t node,
		     grub_disk_read_hook_t read_hook, void *read_hook_data,
		     grub_off_t pos, grub_size_t len, char *buf)
{
  return grub_fshelp_read_file (node->data->disk, node,
				read_hook, read_hook_data,
				pos, len, buf, grub_ext2_read_block,
				grub_cpu_to_le32 (node->inode.size)
				| (((grub_off_t) grub_cpu_to_le32 (node->inode.size_high)) << 32),
				LOG2_EXT2_BLOCK_SIZE (node->data), 0);

}