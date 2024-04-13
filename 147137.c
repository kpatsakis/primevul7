baseURL(Buffer *buf)
{
    if (buf->bufferprop & BP_NO_URL) {
	/* no URL is defined for the buffer */
	return NULL;
    }
    if (buf->baseURL != NULL) {
	/* <BASE> tag is defined in the document */
	return buf->baseURL;
    }
    else if (IS_EMPTY_PARSED_URL(&buf->currentURL))
	return NULL;
    else
	return &buf->currentURL;
}