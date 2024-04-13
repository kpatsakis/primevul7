console_map_init(void)
{
	int i;
	
	for (i = 0; i < MAX_NR_CONSOLES; i++)
		if (vc_cons_allocated(i) && !*vc_cons[i].d->vc_uni_pagedir_loc)
			con_set_default_unimap(vc_cons[i].d);
}