init_mappings(void)
{
#if defined(MSWIN) || defined(MACOS_X)
    int		i;

# if defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  ifdef VIMDLL
    if (!gui.starting)
#  endif
    {
	for (i = 0;
		i < (int)(sizeof(cinitmappings) / sizeof(struct initmap)); ++i)
	    add_map(cinitmappings[i].arg, cinitmappings[i].mode);
    }
# endif
# if defined(FEAT_GUI_MSWIN) || defined(MACOS_X)
    for (i = 0; i < (int)(sizeof(initmappings) / sizeof(struct initmap)); ++i)
	add_map(initmappings[i].arg, initmappings[i].mode);
# endif
#endif
}