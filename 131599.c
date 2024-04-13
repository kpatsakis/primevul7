static void free_canon_ace_list( canon_ace *list_head )
{
	canon_ace *list, *next;

	for (list = list_head; list; list = next) {
		next = list->next;
		DLIST_REMOVE(list_head, list);
		SAFE_FREE(list);
	}
}