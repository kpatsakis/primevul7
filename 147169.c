_put_anchor_news(Buffer *buf, char *p1, char *p2, int line, int pos)
{
    Str tmp;

    if (*p1 == '<') {
	p1++;
	if (*(p2 - 1) == '>')
	    p2--;
    }
    tmp = Strnew_charp("news:");
    Strcat_charp_n(tmp, p1, p2 - p1);
    return registerHref(buf, url_encode(tmp->ptr, baseURL(buf),
					buf->document_charset),
			NULL, NO_REFERER, NULL, '\0', line,
			pos);
}