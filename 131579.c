static void free_inherited_info(struct pai_val *pal)
{
	if (pal) {
		struct pai_entry *paie, *paie_next;
		for (paie = pal->entry_list; paie; paie = paie_next) {
			paie_next = paie->next;
			SAFE_FREE(paie);
		}
		for (paie = pal->def_entry_list; paie; paie = paie_next) {
			paie_next = paie->next;
			SAFE_FREE(paie);
		}
		SAFE_FREE(pal);
	}
}