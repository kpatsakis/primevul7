SCOPE_LIST* license_new_scope_list()
{
	SCOPE_LIST* scopeList;

	scopeList = (SCOPE_LIST*) malloc(sizeof(SCOPE_LIST));
	scopeList->count = 0;
	scopeList->array = NULL;

	return scopeList;
}