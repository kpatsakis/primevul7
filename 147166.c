retrieveCurrentImg(Buffer *buf)
{
    if (buf->currentLine == NULL)
	return NULL;
    return retrieveAnchor(buf->img, buf->currentLine->linenumber, buf->pos);
}