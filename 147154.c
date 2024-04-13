registerName(Buffer *buf, char *url, int line, int pos)
{
    Anchor *a;
    buf->name = putAnchor(buf->name, url, NULL, &a, NULL, NULL, '\0', line,
			  pos);
    return a;
}