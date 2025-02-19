int del_match_busid(char *busid)
{
	int idx;
	int ret = -1;

	spin_lock(&busid_table_lock);
	idx = get_busid_idx(busid);
	if (idx < 0)
		goto out;

	/* found */
	ret = 0;

	spin_lock(&busid_table[idx].busid_lock);

	if (busid_table[idx].status == STUB_BUSID_OTHER)
		memset(busid_table[idx].name, 0, BUSID_SIZE);

	if ((busid_table[idx].status != STUB_BUSID_OTHER) &&
	    (busid_table[idx].status != STUB_BUSID_ADDED))
		busid_table[idx].status = STUB_BUSID_REMOV;

	spin_unlock(&busid_table[idx].busid_lock);
out:
	spin_unlock(&busid_table_lock);

	return ret;
}