GRUB_MOD_INIT(ext2)
{
  grub_fs_register (&grub_ext2_fs);
  my_mod = mod;
}