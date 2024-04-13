estack_sfile(estack_arg_T which UNUSED)
{
    estack_T	*entry;
#ifdef FEAT_EVAL
    garray_T	ga;
    size_t	len;
    int		idx;
    etype_T	last_type = ETYPE_SCRIPT;
    char	*type_name;
#endif

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;
#ifdef FEAT_EVAL
    if (which == ESTACK_SFILE && entry->es_type != ETYPE_UFUNC)
#endif
    {
	if (entry->es_name == NULL)
	    return NULL;
	return vim_strsave(entry->es_name);
    }
#ifdef FEAT_EVAL
    // expand('<sfile>') works in a function for backwards compatibility, but
    // may give an unexpected result.  Disallow it in Vim 9 script.
    if (which == ESTACK_SFILE && in_vim9script())
    {
	int  save_emsg_off = emsg_off;

	if (emsg_off == 1)
	    // f_expand() silences errors but we do want this one
	    emsg_off = 0;
	emsg(_(e_cannot_expand_sfile_in_vim9_function));
	emsg_off = save_emsg_off;
	return NULL;
    }

    // If evaluated in a function or autocommand, return the path of the script
    // where it is defined, at script level the current script path is returned
    // instead.
    if (which == ESTACK_SCRIPT)
    {
	entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;
	// Walk the stack backwards, starting from the current frame.
	for (idx = exestack.ga_len - 1; idx >= 0; --idx, --entry)
	{
	    if (entry->es_type == ETYPE_UFUNC)
	    {
		sctx_T *def_ctx = &entry->es_info.ufunc->uf_script_ctx;

		if (def_ctx->sc_sid > 0)
		    return vim_strsave(SCRIPT_ITEM(def_ctx->sc_sid)->sn_name);
		else
		    return NULL;
	    }
	    else if (entry->es_type == ETYPE_AUCMD)
	    {
		sctx_T *def_ctx = acp_script_ctx(entry->es_info.aucmd);

		if (def_ctx->sc_sid > 0)
		    return vim_strsave(SCRIPT_ITEM(def_ctx->sc_sid)->sn_name);
		else
		    return NULL;
	    }
	    else if (entry->es_type == ETYPE_SCRIPT)
	    {
		return vim_strsave(entry->es_name);
	    }
	}
	return NULL;
    }

    // Give information about each stack entry up to the root.
    // For a function we compose the call stack, as it was done in the past:
    //   "function One[123]..Two[456]..Three"
    ga_init2(&ga, sizeof(char), 100);
    for (idx = 0; idx < exestack.ga_len; ++idx)
    {
	entry = ((estack_T *)exestack.ga_data) + idx;
	if (entry->es_name != NULL)
	{
	    long    lnum = 0;
	    char    *dots;

	    len = STRLEN(entry->es_name) + 15;
	    type_name = "";
	    if (entry->es_type != last_type)
	    {
		switch (entry->es_type)
		{
		    case ETYPE_SCRIPT: type_name = "script "; break;
		    case ETYPE_UFUNC: type_name = "function "; break;
		    default: type_name = ""; break;
		}
		last_type = entry->es_type;
	    }
	    len += STRLEN(type_name);
	    if (ga_grow(&ga, (int)len) == FAIL)
		break;
	    if (idx == exestack.ga_len - 1)
		lnum = which == ESTACK_STACK ? SOURCING_LNUM : 0;
	    else
		lnum = entry->es_lnum;
	    dots = idx == exestack.ga_len - 1 ? "" : "..";
	    if (lnum == 0)
		// For the bottom entry of <sfile>: do not add the line number,
		// it is used in <slnum>.  Also leave it out when the number is
		// not set.
		vim_snprintf((char *)ga.ga_data + ga.ga_len, len, "%s%s%s",
				type_name, entry->es_name, dots);
	    else
		vim_snprintf((char *)ga.ga_data + ga.ga_len, len, "%s%s[%ld]%s",
				    type_name, entry->es_name, lnum, dots);
	    ga.ga_len += (int)STRLEN((char *)ga.ga_data + ga.ga_len);
	}
    }

    return (char_u *)ga.ga_data;
#endif
}