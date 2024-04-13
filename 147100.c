chkExternalURIBuffer(Buffer *buf)
{
    int i;
    struct table2 *ump;

    for (i = 0; (ump = urimethods[i]) != NULL; i++) {
	for (; ump->item1 != NULL; ump++) {
	    reAnchor(buf, Sprintf("%s:%s", ump->item1, URI_PATTERN)->ptr);
	}
    }
    for (ump = default_urimethods; ump->item1 != NULL; ump++) {
	reAnchor(buf, Sprintf("%s:%s", ump->item1, URI_PATTERN)->ptr);
    }
}