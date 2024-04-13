static bool check_pai_ok(char *pai_buf, size_t pai_buf_data_size)
{
	uint16 num_entries;
	uint16 num_def_entries;

	if (pai_buf_data_size < PAI_ENTRIES_BASE) {
		/* Corrupted - too small. */
		return False;
	}

	if (CVAL(pai_buf,PAI_VERSION_OFFSET) != PAI_VERSION)
		return False;

	num_entries = SVAL(pai_buf,PAI_NUM_ENTRIES_OFFSET);
	num_def_entries = SVAL(pai_buf,PAI_NUM_DEFAULT_ENTRIES_OFFSET);

	/* Check the entry lists match. */
	/* Each entry is 5 bytes (type plus 4 bytes of uid or gid). */

	if (((num_entries + num_def_entries)*PAI_ENTRY_LENGTH) + PAI_ENTRIES_BASE != pai_buf_data_size)
		return False;

	return True;
}