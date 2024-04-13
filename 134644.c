GRUB_MOD_FINI(ext2)
{
  grub_fs_unregister (&grub_ext2_fs);
}