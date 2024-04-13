link_list_panel(Buffer *buf)
{
    LinkList *l;
    AnchorList *al;
    Anchor *a;
    FormItemList *fi;
    int i;
    char *t, *u, *p;
    ParsedURL pu;
    /* FIXME: gettextize? */
    Str tmp = Strnew_charp("<title>Link List</title>\
<h1 align=center>Link List</h1>\n");

    if (buf->bufferprop & BP_INTERNAL ||
	(buf->linklist == NULL && buf->href == NULL && buf->img == NULL)) {
	return NULL;
    }

    if (buf->linklist) {
	Strcat_charp(tmp, "<hr><h2>Links</h2>\n<ol>\n");
	for (l = buf->linklist; l; l = l->next) {
	    if (l->url) {
		parseURL2(l->url, &pu, baseURL(buf));
		p = parsedURL2Str(&pu)->ptr;
		u = html_quote(p);
		if (DecodeURL)
		    p = html_quote(url_decode2(p, buf));
		else
		    p = u;
	    }
	    else
		u = p = "";
	    if (l->type == LINK_TYPE_REL)
		t = " [Rel]";
	    else if (l->type == LINK_TYPE_REV)
		t = " [Rev]";
	    else
		t = "";
	    t = Sprintf("%s%s\n", l->title ? l->title : "", t)->ptr;
	    t = html_quote(t);
	    Strcat_m_charp(tmp, "<li><a href=\"", u, "\">", t, "</a><br>", p,
			   "\n", NULL);
	}
	Strcat_charp(tmp, "</ol>\n");
    }

    if (buf->href) {
	Strcat_charp(tmp, "<hr><h2>Anchors</h2>\n<ol>\n");
	al = buf->href;
	for (i = 0; i < al->nanchor; i++) {
	    a = &al->anchors[i];
	    if (a->hseq < 0 || a->slave)
		continue;
	    parseURL2(a->url, &pu, baseURL(buf));
	    p = parsedURL2Str(&pu)->ptr;
	    u = html_quote(p);
	    if (DecodeURL)
		p = html_quote(url_decode2(p, buf));
	    else
		p = u;
	    t = getAnchorText(buf, al, a);
	    t = t ? html_quote(t) : "";
	    Strcat_m_charp(tmp, "<li><a href=\"", u, "\">", t, "</a><br>", p,
			   "\n", NULL);
	}
	Strcat_charp(tmp, "</ol>\n");
    }

    if (buf->img) {
	Strcat_charp(tmp, "<hr><h2>Images</h2>\n<ol>\n");
	al = buf->img;
	for (i = 0; i < al->nanchor; i++) {
	    a = &al->anchors[i];
	    if (a->slave)
		continue;
	    parseURL2(a->url, &pu, baseURL(buf));
	    p = parsedURL2Str(&pu)->ptr;
	    u = html_quote(p);
	    if (DecodeURL)
		p = html_quote(url_decode2(p, buf));
	    else
		p = u;
	    if (a->title && *a->title)
		t = html_quote(a->title);
	    else
		t = html_quote(url_decode2(a->url, buf));
	    Strcat_m_charp(tmp, "<li><a href=\"", u, "\">", t, "</a><br>", p,
			   "\n", NULL);
	    a = retrieveAnchor(buf->formitem, a->start.line, a->start.pos);
	    if (!a)
		continue;
	    fi = (FormItemList *)a->url;
	    fi = fi->parent->item;
	    if (fi->parent->method == FORM_METHOD_INTERNAL &&
		!Strcmp_charp(fi->parent->action, "map") && fi->value) {
		MapList *ml = searchMapList(buf, fi->value->ptr);
		ListItem *mi;
		MapArea *m;
		if (!ml)
		    continue;
		Strcat_charp(tmp, "<br>\n<b>Image map</b>\n<ol>\n");
		for (mi = ml->area->first; mi != NULL; mi = mi->next) {
		    m = (MapArea *) mi->ptr;
		    if (!m)
			continue;
		    parseURL2(m->url, &pu, baseURL(buf));
		    p = parsedURL2Str(&pu)->ptr;
		    u = html_quote(p);
		    if (DecodeURL)
			p = html_quote(url_decode2(p, buf));
		    else
			p = u;
		    if (m->alt && *m->alt)
			t = html_quote(m->alt);
		    else
			t = html_quote(url_decode2(m->url, buf));
		    Strcat_m_charp(tmp, "<li><a href=\"", u, "\">", t,
				   "</a><br>", p, "\n", NULL);
		}
		Strcat_charp(tmp, "</ol>\n");
	    }
	}
	Strcat_charp(tmp, "</ol>\n");
    }

    return loadHTMLString(tmp);
}