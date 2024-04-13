registerHref(Buffer *buf, char *url, char *target, char *referer, char *title,
	     unsigned char key, int line, int pos)
{
    Anchor *a;
    buf->href = putAnchor(buf->href, url, target, &a, referer, title, key,
			  line, pos);
    return a;
}