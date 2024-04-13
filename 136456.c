aspath_print_vty (struct vty *vty, const char *format, struct aspath *as, const char * suffix)
{
  assert (format);
  vty_out (vty, format, as->str);
  if (as->str_len && strlen (suffix))
    vty_out (vty, "%s", suffix);
}