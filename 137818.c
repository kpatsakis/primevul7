xsltFreeStylePreComp(xsltStylePreCompPtr comp) {
    if (comp == NULL)
	return;
#ifdef XSLT_REFACTORED
    /*
    * URGENT TODO: Implement destructors.
    */
    switch (comp->type) {
	case XSLT_FUNC_LITERAL_RESULT_ELEMENT:
	    break;
	case XSLT_FUNC_COPY:
            break;
        case XSLT_FUNC_SORT: {
		xsltStyleItemSortPtr item = (xsltStyleItemSortPtr) comp;
		if (item->locale != (xsltLocale)0)
		    xsltFreeLocale(item->locale);
		if (item->comp != NULL)
		    xmlXPathFreeCompExpr(item->comp);
	    }
            break;
        case XSLT_FUNC_TEXT:
            break;
        case XSLT_FUNC_ELEMENT:
            break;
        case XSLT_FUNC_ATTRIBUTE:
            break;
        case XSLT_FUNC_COMMENT:
            break;
        case XSLT_FUNC_PI:
	    break;
        case XSLT_FUNC_COPYOF: {
		xsltStyleItemCopyOfPtr item = (xsltStyleItemCopyOfPtr) comp;
		if (item->comp != NULL)
		    xmlXPathFreeCompExpr(item->comp);
	    }
            break;
        case XSLT_FUNC_VALUEOF: {
		xsltStyleItemValueOfPtr item = (xsltStyleItemValueOfPtr) comp;
		if (item->comp != NULL)
		    xmlXPathFreeCompExpr(item->comp);
	    }
            break;
        case XSLT_FUNC_NUMBER: {
                xsltStyleItemNumberPtr item = (xsltStyleItemNumberPtr) comp;
                if (item->numdata.countPat != NULL)
                    xsltFreeCompMatchList(item->numdata.countPat);
                if (item->numdata.fromPat != NULL)
                    xsltFreeCompMatchList(item->numdata.fromPat);
            }
            break;
        case XSLT_FUNC_APPLYIMPORTS:
            break;
        case XSLT_FUNC_CALLTEMPLATE:
            break;
        case XSLT_FUNC_APPLYTEMPLATES: {
		xsltStyleItemApplyTemplatesPtr item =
		    (xsltStyleItemApplyTemplatesPtr) comp;
		if (item->comp != NULL)
		    xmlXPathFreeCompExpr(item->comp);
	    }
            break;
        case XSLT_FUNC_CHOOSE:
            break;
        case XSLT_FUNC_IF: {
		xsltStyleItemIfPtr item = (xsltStyleItemIfPtr) comp;
		if (item->comp != NULL)
		    xmlXPathFreeCompExpr(item->comp);
	    }
            break;
        case XSLT_FUNC_FOREACH: {
		xsltStyleItemForEachPtr item =
		    (xsltStyleItemForEachPtr) comp;
		if (item->comp != NULL)
		    xmlXPathFreeCompExpr(item->comp);
	    }
            break;
        case XSLT_FUNC_DOCUMENT:
            break;
	case XSLT_FUNC_WITHPARAM: {
		xsltStyleItemWithParamPtr item =
		    (xsltStyleItemWithParamPtr) comp;
		if (item->comp != NULL)
		    xmlXPathFreeCompExpr(item->comp);
	    }
	    break;
	case XSLT_FUNC_PARAM: {
		xsltStyleItemParamPtr item =
		    (xsltStyleItemParamPtr) comp;
		if (item->comp != NULL)
		    xmlXPathFreeCompExpr(item->comp);
	    }
	    break;
	case XSLT_FUNC_VARIABLE: {
		xsltStyleItemVariablePtr item =
		    (xsltStyleItemVariablePtr) comp;
		if (item->comp != NULL)
		    xmlXPathFreeCompExpr(item->comp);
	    }
	    break;
	case XSLT_FUNC_WHEN: {
		xsltStyleItemWhenPtr item =
		    (xsltStyleItemWhenPtr) comp;
		if (item->comp != NULL)
		    xmlXPathFreeCompExpr(item->comp);
	    }
	    break;
	case XSLT_FUNC_OTHERWISE:
	case XSLT_FUNC_FALLBACK:
	case XSLT_FUNC_MESSAGE:
	case XSLT_FUNC_INCLUDE:
	case XSLT_FUNC_ATTRSET:

	    break;
	default:
	    /* TODO: Raise error. */
	    break;
    }
#else
    if (comp->locale != (xsltLocale)0)
	xsltFreeLocale(comp->locale);
    if (comp->comp != NULL)
	xmlXPathFreeCompExpr(comp->comp);
    if (comp->numdata.countPat != NULL)
        xsltFreeCompMatchList(comp->numdata.countPat);
    if (comp->numdata.fromPat != NULL)
        xsltFreeCompMatchList(comp->numdata.fromPat);
    if (comp->nsList != NULL)
	xmlFree(comp->nsList);
#endif

    xmlFree(comp);
}