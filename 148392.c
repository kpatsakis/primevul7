const char* sldns_get_errorstr_parse(int e)
{
	sldns_lookup_table *lt;
	lt = sldns_lookup_by_id(sldns_wireparse_errors, LDNS_WIREPARSE_ERROR(e));
	return lt?lt->name:"unknown error";
}