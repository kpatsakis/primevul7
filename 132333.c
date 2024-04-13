formRecheckRadio(Anchor *a, Buffer *buf, FormItemList *fi)
{
    int i;
    Anchor *a2;
    FormItemList *f2;

    for (i = 0; i < buf->formitem->nanchor; i++) {
	a2 = &buf->formitem->anchors[i];
	f2 = (FormItemList *)a2->url;
	if (f2->parent == fi->parent && f2 != fi &&
	    f2->type == FORM_INPUT_RADIO && Strcmp(f2->name, fi->name) == 0) {
	    f2->checked = 0;
	    formUpdateBuffer(a2, buf, f2);
	}
    }
    fi->checked = 1;
    formUpdateBuffer(a, buf, fi);
}