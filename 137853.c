exsltSaxonSystemIdFunction(xmlXPathParserContextPtr ctxt, int nargs)
{
    if (ctxt == NULL)
        return;
    if (nargs != 0) {
        xmlXPathSetArityError(ctxt);
        return;
    }

    if ((ctxt->context) && (ctxt->context->doc) &&
        (ctxt->context->doc->URL))
	valuePush(ctxt, xmlXPathNewString(ctxt->context->doc->URL));
    else
	valuePush(ctxt, xmlXPathNewString(BAD_CAST ""));
}