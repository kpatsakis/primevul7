xsltNumberFormatGetAnyLevel(xsltTransformContextPtr context,
			    xmlNodePtr node,
			    xsltCompMatchPtr countPat,
			    xsltCompMatchPtr fromPat,
			    double *array)
{
    int amount = 0;
    int cnt = 0;
    xmlNodePtr cur = node;

    while (cur != NULL) {
	/* process current node */
	if (xsltTestCompMatchCount(context, cur, countPat, node))
	    cnt++;
	if ((fromPat != NULL) &&
	    xsltTestCompMatchList(context, cur, fromPat)) {
	    break; /* while */
	}

	/* Skip to next preceding or ancestor */
	if ((cur->type == XML_DOCUMENT_NODE) ||
#ifdef LIBXML_DOCB_ENABLED
            (cur->type == XML_DOCB_DOCUMENT_NODE) ||
#endif
            (cur->type == XML_HTML_DOCUMENT_NODE))
	    break; /* while */

        if (cur->type == XML_NAMESPACE_DECL) {
            /*
            * The XPath module stores the parent of a namespace node in
            * the ns->next field.
            */
            cur = (xmlNodePtr) ((xmlNsPtr) cur)->next;
        } else if (cur->type == XML_ATTRIBUTE_NODE) {
            cur = cur->parent;
        } else {
            while ((cur->prev != NULL) && ((cur->prev->type == XML_DTD_NODE) ||
                   (cur->prev->type == XML_XINCLUDE_START) ||
                   (cur->prev->type == XML_XINCLUDE_END)))
                cur = cur->prev;
            if (cur->prev != NULL) {
                for (cur = cur->prev; cur->last != NULL; cur = cur->last);
            } else {
                cur = cur->parent;
            }
        }
    }

    array[amount++] = (double) cnt;

    return(amount);
}