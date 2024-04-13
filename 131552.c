static bool get_inherited_flag(struct pai_val *pal, canon_ace *ace_entry, bool default_ace)
{
	struct pai_entry *paie;

	if (!pal)
		return False;

	/* If the entry exists it is inherited. */
	for (paie = (default_ace ? pal->def_entry_list : pal->entry_list); paie; paie = paie->next) {
		if (ace_entry->owner_type == paie->owner_type &&
				get_entry_val(ace_entry) == get_pai_entry_val(paie))
			return True;
	}
	return False;
}