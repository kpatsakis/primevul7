reAnchorNewsheader(Buffer *buf)
{
    Line *l;
    char *p, *p1, *p2;
    static char *header_mid[] = {
	"Message-Id:", "References:", "In-Reply-To:", NULL
    };
    static char *header_group[] = {
	"Newsgroups:", NULL
    };
    char **header, **q;
    int i, search = FALSE;

    if (!buf || !buf->firstLine)
	return NULL;
    for (i = 0; i <= 1; i++) {
	if (i == 0) {
	    regexCompile("<[!-;=?-~]+@[a-zA-Z0-9\\.\\-_]+>", 1);
	    header = header_mid;
	}
	else {
	    regexCompile("[a-zA-Z0-9\\.\\-_]+", 1);
	    header = header_group;
	}
	for (l = buf->firstLine; l != NULL && l->real_linenumber == 0;
	     l = l->next) {
	    if (l->bpos)
		continue;
	    p = l->lineBuf;
	    if (!IS_SPACE(*p)) {
		search = FALSE;
		for (q = header; *q; q++) {
		    if (!strncasecmp(p, *q, strlen(*q))) {
			search = TRUE;
			p = strchr(p, ':') + 1;
			break;
		    }
		}
	    }
	    if (!search)
		continue;
	    for (;;) {
		if (regexMatch(p, &l->lineBuf[l->size] - p, p == l->lineBuf)
		    == 1) {
		    matchedPosition(&p1, &p2);
		    p = reAnchorPos(buf, l, p1, p2, _put_anchor_news);
		}
		else
		    break;
	    }
	}
    }
    reseq_anchor(buf);
    return NULL;
}