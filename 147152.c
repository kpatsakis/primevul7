registerImg(Buffer *buf, char *url, char *title, int line, int pos)
{
    Anchor *a;
    buf->img = putAnchor(buf->img, url, NULL, &a, NULL, title, '\0', line,
			 pos);
    return a;
}