static unsigned int num_inherited_entries(canon_ace *ace_list)
{
	unsigned int num_entries = 0;

	for (; ace_list; ace_list = ace_list->next)
		if (ace_list->inherited)
			num_entries++;
	return num_entries;
}