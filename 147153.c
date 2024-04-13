_put_anchor_all(Buffer *buf, char *p1, char *p2, int line, int pos)
{
    Str tmp;

    tmp = Strnew_charp_n(p1, p2 - p1);
    return registerHref(buf, url_encode(tmp->ptr, baseURL(buf),
					buf->document_charset),
			NULL, NO_REFERER, NULL,
			'\0', line, pos);
}