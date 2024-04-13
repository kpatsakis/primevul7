registerForm(Buffer *buf, FormList *flist, struct parsed_tag *tag, int line,
	     int pos)
{
    Anchor *a;
    FormItemList *fi;

    fi = formList_addInput(flist, tag);
    if (fi == NULL)
	return NULL;
    buf->formitem = putAnchor(buf->formitem, (char *)fi, flist->target, &a,
			      NULL, NULL, '\0', line, pos);
    return a;
}