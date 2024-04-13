retrieveCurrentAnchor(Buffer *buf)
{
    if (buf->currentLine == NULL)
	return NULL;
    return retrieveAnchor(buf->href, buf->currentLine->linenumber, buf->pos);
}