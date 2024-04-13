static char *create_pai_buf(canon_ace *file_ace_list, canon_ace *dir_ace_list, bool pai_protected, size_t *store_size)
{
	char *pai_buf = NULL;
	canon_ace *ace_list = NULL;
	char *entry_offset = NULL;
	unsigned int num_entries = 0;
	unsigned int num_def_entries = 0;

	for (ace_list = file_ace_list; ace_list; ace_list = ace_list->next)
		if (ace_list->inherited)
			num_entries++;

	for (ace_list = dir_ace_list; ace_list; ace_list = ace_list->next)
		if (ace_list->inherited)
			num_def_entries++;

	DEBUG(10,("create_pai_buf: num_entries = %u, num_def_entries = %u\n", num_entries, num_def_entries ));

	*store_size = PAI_ENTRIES_BASE + ((num_entries + num_def_entries)*PAI_ENTRY_LENGTH);

	pai_buf = (char *)SMB_MALLOC(*store_size);
	if (!pai_buf) {
		return NULL;
	}

	/* Set up the header. */
	memset(pai_buf, '\0', PAI_ENTRIES_BASE);
	SCVAL(pai_buf,PAI_VERSION_OFFSET,PAI_VERSION);
	SCVAL(pai_buf,PAI_FLAG_OFFSET,(pai_protected ? PAI_ACL_FLAG_PROTECTED : 0));
	SSVAL(pai_buf,PAI_NUM_ENTRIES_OFFSET,num_entries);
	SSVAL(pai_buf,PAI_NUM_DEFAULT_ENTRIES_OFFSET,num_def_entries);

	entry_offset = pai_buf + PAI_ENTRIES_BASE;

	for (ace_list = file_ace_list; ace_list; ace_list = ace_list->next) {
		if (ace_list->inherited) {
			uint8 type_val = (unsigned char)ace_list->owner_type;
			uint32 entry_val = get_entry_val(ace_list);

			SCVAL(entry_offset,0,type_val);
			SIVAL(entry_offset,1,entry_val);
			entry_offset += PAI_ENTRY_LENGTH;
		}
	}

	for (ace_list = dir_ace_list; ace_list; ace_list = ace_list->next) {
		if (ace_list->inherited) {
			uint8 type_val = (unsigned char)ace_list->owner_type;
			uint32 entry_val = get_entry_val(ace_list);

			SCVAL(entry_offset,0,type_val);
			SIVAL(entry_offset,1,entry_val);
			entry_offset += PAI_ENTRY_LENGTH;
		}
	}

	return pai_buf;
}