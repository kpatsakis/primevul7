addMultirowsImg(Buffer *buf, AnchorList *al)
{
    int i, j, k, col, ecol, pos;
    Image *img;
    Anchor a_img, a_href, a_form, *a;
    Line *l, *ls;

    if (al == NULL || al->nanchor == 0)
	return;
    for (i = 0; i < al->nanchor; i++) {
	a_img = al->anchors[i];
	img = a_img.image;
	if (a_img.hseq < 0 || !img || img->rows <= 1)
	    continue;
	for (l = buf->firstLine; l != NULL; l = l->next) {
	    if (l->linenumber == img->y)
		break;
	}
	if (!l)
	    continue;
	if (a_img.y == a_img.start.line)
	    ls = l;
	else {
	    for (ls = l; ls != NULL;
		 ls = (a_img.y < a_img.start.line) ? ls->next : ls->prev) {
		if (ls->linenumber == a_img.start.line)
		    break;
	    }
	    if (!ls)
		continue;
	}
	a = retrieveAnchor(buf->href, a_img.start.line, a_img.start.pos);
	if (a)
	    a_href = *a;
	else
	    a_href.url = NULL;
	a = retrieveAnchor(buf->formitem, a_img.start.line, a_img.start.pos);
	if (a)
	    a_form = *a;
	else
	    a_form.url = NULL;
	col = COLPOS(ls, a_img.start.pos);
	ecol = COLPOS(ls, a_img.end.pos);
	for (j = 0; l && j < img->rows; l = l->next, j++) {
	    if (a_img.start.line == l->linenumber)
		continue;
	    pos = columnPos(l, col);
	    a = registerImg(buf, a_img.url, a_img.title, l->linenumber, pos);
	    a->hseq = -a_img.hseq;
	    a->slave = TRUE;
	    a->image = img;
	    a->end.pos = pos + ecol - col;
	    for (k = pos; k < a->end.pos; k++)
		l->propBuf[k] |= PE_IMAGE;
	    if (a_href.url) {
		a = registerHref(buf, a_href.url, a_href.target,
				 a_href.referer, a_href.title,
				 a_href.accesskey, l->linenumber, pos);
		a->hseq = a_href.hseq;
		a->slave = TRUE;
		a->end.pos = pos + ecol - col;
		for (k = pos; k < a->end.pos; k++)
		    l->propBuf[k] |= PE_ANCHOR;
	    }
	    if (a_form.url) {
		buf->formitem = putAnchor(buf->formitem, a_form.url,
					  a_form.target, &a, NULL, NULL, '\0',
					  l->linenumber, pos);
		a->hseq = a_form.hseq;
		a->end.pos = pos + ecol - col;
	    }
	}
	img->rows = 0;
    }
}