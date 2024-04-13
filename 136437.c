aspath_print_all_vty (struct vty *vty)
{
  hash_iterate (ashash, 
		(void (*) (struct hash_backet *, void *))
		aspath_show_all_iterator,
		vty);
}