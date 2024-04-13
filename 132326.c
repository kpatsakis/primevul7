updateSelectOption(FormItemList *fi, FormSelectOptionItem *item)
{
    int i;

    if (fi == NULL || item == NULL)
	return;
    for (i = 0; item != NULL; i++, item = item->next) {
	if (i == fi->selected)
	    item->checked = TRUE;
	else
	    item->checked = FALSE;
    }
}