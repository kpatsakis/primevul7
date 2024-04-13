estack_top_is_ufunc(ufunc_T *ufunc, long lnum)
{
    estack_T *entry;

    if (exestack.ga_len == 0)
	return FALSE;
    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;
    return entry->es_type == ETYPE_UFUNC
	&& STRCMP( entry->es_name, ufunc->uf_name_exp != NULL
				    ? ufunc->uf_name_exp : ufunc->uf_name) == 0
	&& entry->es_lnum == lnum;
}