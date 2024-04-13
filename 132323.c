preFormUpdateBuffer(Buffer *buf)
{
    struct pre_form *pf;
    struct pre_form_item *pi;
    int i;
    Anchor *a;
    FormList *fl;
    FormItemList *fi;
#ifdef MENU_SELECT
    FormSelectOptionItem *opt;
    int j;
#endif

    if (!buf || !buf->formitem || !PreForm)
	return;

    for (pf = PreForm; pf; pf = pf->next) {
	if (pf->re_url) {
	    Str url = parsedURL2Str(&buf->currentURL);
	    if (!RegexMatch(pf->re_url, url->ptr, url->length, 1))
		continue;
	}
	else if (pf->url) {
	    if (Strcmp_charp(parsedURL2Str(&buf->currentURL), pf->url))
		continue;
	}
	else
	    continue;
	for (i = 0; i < buf->formitem->nanchor; i++) {
	    a = &buf->formitem->anchors[i];
	    fi = (FormItemList *)a->url;
	    fl = fi->parent;
	    if (pf->name && (!fl->name || strcmp(fl->name, pf->name)))
		continue;
	    if (pf->action
		&& (!fl->action || Strcmp_charp(fl->action, pf->action)))
		continue;
	    for (pi = pf->item; pi; pi = pi->next) {
		if (pi->type != fi->type)
		    continue;
		if (pi->type == FORM_INPUT_SUBMIT ||
		    pi->type == FORM_INPUT_IMAGE) {
		    if ((!pi->name || !*pi->name ||
			 (fi->name && !Strcmp_charp(fi->name, pi->name))) &&
			(!pi->value || !*pi->value ||
			 (fi->value && !Strcmp_charp(fi->value, pi->value))))
			buf->submit = a;
		    continue;
		}
		if (!pi->name || !fi->name || Strcmp_charp(fi->name, pi->name))
		    continue;
		switch (pi->type) {
		case FORM_INPUT_TEXT:
		case FORM_INPUT_FILE:
		case FORM_INPUT_PASSWORD:
		case FORM_TEXTAREA:
		    fi->value = Strnew_charp(pi->value);
		    formUpdateBuffer(a, buf, fi);
		    break;
		case FORM_INPUT_CHECKBOX:
		    if (pi->value && fi->value &&
			!Strcmp_charp(fi->value, pi->value)) {
			fi->checked = pi->checked;
			formUpdateBuffer(a, buf, fi);
		    }
		    break;
		case FORM_INPUT_RADIO:
		    if (pi->value && fi->value &&
			!Strcmp_charp(fi->value, pi->value))
			formRecheckRadio(a, buf, fi);
		    break;
#ifdef MENU_SELECT
		case FORM_SELECT:
		    for (j = 0, opt = fi->select_option; opt != NULL;
			 j++, opt = opt->next) {
			if (pi->value && opt->value &&
			    !Strcmp_charp(opt->value, pi->value)) {
			    fi->selected = j;
			    fi->value = opt->value;
			    fi->label = opt->label;
			    updateSelectOption(fi, fi->select_option);
			    formUpdateBuffer(a, buf, fi);
			    break;
			}
		    }
		    break;
#endif
		}
	    }
	}
    }
}