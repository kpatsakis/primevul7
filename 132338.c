formResetBuffer(Buffer *buf, AnchorList *formitem)
{
    int i;
    Anchor *a;
    FormItemList *f1, *f2;

    if (buf == NULL || buf->formitem == NULL || formitem == NULL)
	return;
    for (i = 0; i < buf->formitem->nanchor && i < formitem->nanchor; i++) {
	a = &buf->formitem->anchors[i];
	if (a->y != a->start.line)
	    continue;
	f1 = (FormItemList *)a->url;
	f2 = (FormItemList *)formitem->anchors[i].url;
	if (f1->type != f2->type ||
	    strcmp(((f1->name == NULL) ? "" : f1->name->ptr),
		   ((f2->name == NULL) ? "" : f2->name->ptr)))
	    break;		/* What's happening */
	switch (f1->type) {
	case FORM_INPUT_TEXT:
	case FORM_INPUT_PASSWORD:
	case FORM_INPUT_FILE:
	case FORM_TEXTAREA:
	    f1->value = f2->value;
	    f1->init_value = f2->init_value;
	    break;
	case FORM_INPUT_CHECKBOX:
	case FORM_INPUT_RADIO:
	    f1->checked = f2->checked;
	    f1->init_checked = f2->init_checked;
	    break;
	case FORM_SELECT:
#ifdef MENU_SELECT
	    f1->select_option = f2->select_option;
	    f1->value = f2->value;
	    f1->label = f2->label;
	    f1->selected = f2->selected;
	    f1->init_value = f2->init_value;
	    f1->init_label = f2->init_label;
	    f1->init_selected = f2->init_selected;
#endif				/* MENU_SELECT */
	    break;
	default:
	    continue;
	}
	formUpdateBuffer(a, buf, f1);
    }
}