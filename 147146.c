retrieveCurrentForm(Buffer *buf)
{
    if (buf->currentLine == NULL)
	return NULL;
    return retrieveAnchor(buf->formitem,
			  buf->currentLine->linenumber, buf->pos);
}