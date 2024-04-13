void license_print_scope_list(SCOPE_LIST* scopeList)
{
	int index;
	LICENSE_BLOB* scope;

	fprintf(stderr, "ScopeList (%d):\n", scopeList->count);

	for (index = 0; index < scopeList->count; index++)
	{
		scope = &scopeList->array[index];
		fprintf(stderr, "\t%s\n", (char*) scope->data);
	}
}