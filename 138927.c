set_context_in_map_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    int		forceit,	/* TRUE if '!' given */
    int		isabbrev,	/* TRUE if abbreviation */
    int		isunmap,	/* TRUE if unmap/unabbrev command */
    cmdidx_T	cmdidx)
{
    if (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    else
    {
	if (isunmap)
	    expand_mapmodes = get_map_mode(&cmd, forceit || isabbrev);
	else
	{
	    expand_mapmodes = INSERT + CMDLINE;
	    if (!isabbrev)
		expand_mapmodes += VISUAL + SELECTMODE + NORMAL + OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
#ifdef FEAT_LOCALMAP
	expand_buffer = FALSE;
#endif
	for (;;)
	{
#ifdef FEAT_LOCALMAP
	    if (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_buffer = TRUE;
		arg = skipwhite(arg + 8);
		continue;
	    }
#endif
	    if (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    if (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    if (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    if (STRNCMP(arg, "<special>", 9) == 0)
	    {
		arg = skipwhite(arg + 9);
		continue;
	    }
#ifdef FEAT_EVAL
	    if (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    if (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		continue;
	    }
#endif
	    break;
	}
	xp->xp_pattern = arg;
    }

    return NULL;
}