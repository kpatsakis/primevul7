xsltNewStylePreComp(xsltStylesheetPtr style, xsltStyleType type) {
    xsltStylePreCompPtr cur;
#ifdef XSLT_REFACTORED
    size_t size;
#endif

    if (style == NULL)
        return(NULL);

#ifdef XSLT_REFACTORED
    /*
    * URGENT TODO: Use specialized factory functions in order
    *   to avoid this ugliness.
    */
    switch (type) {
        case XSLT_FUNC_COPY:
            size = sizeof(xsltStyleItemCopy); break;
        case XSLT_FUNC_SORT:
            size = sizeof(xsltStyleItemSort); break;
        case XSLT_FUNC_TEXT:
            size = sizeof(xsltStyleItemText); break;
        case XSLT_FUNC_ELEMENT:
            size = sizeof(xsltStyleItemElement); break;
        case XSLT_FUNC_ATTRIBUTE:
            size = sizeof(xsltStyleItemAttribute); break;
        case XSLT_FUNC_COMMENT:
            size = sizeof(xsltStyleItemComment); break;
        case XSLT_FUNC_PI:
            size = sizeof(xsltStyleItemPI); break;
        case XSLT_FUNC_COPYOF:
            size = sizeof(xsltStyleItemCopyOf); break;
        case XSLT_FUNC_VALUEOF:
            size = sizeof(xsltStyleItemValueOf); break;;
        case XSLT_FUNC_NUMBER:
            size = sizeof(xsltStyleItemNumber); break;
        case XSLT_FUNC_APPLYIMPORTS:
            size = sizeof(xsltStyleItemApplyImports); break;
        case XSLT_FUNC_CALLTEMPLATE:
            size = sizeof(xsltStyleItemCallTemplate); break;
        case XSLT_FUNC_APPLYTEMPLATES:
            size = sizeof(xsltStyleItemApplyTemplates); break;
        case XSLT_FUNC_CHOOSE:
            size = sizeof(xsltStyleItemChoose); break;
        case XSLT_FUNC_IF:
            size = sizeof(xsltStyleItemIf); break;
        case XSLT_FUNC_FOREACH:
            size = sizeof(xsltStyleItemForEach); break;
        case XSLT_FUNC_DOCUMENT:
            size = sizeof(xsltStyleItemDocument); break;
	case XSLT_FUNC_WITHPARAM:
	    size = sizeof(xsltStyleItemWithParam); break;
	case XSLT_FUNC_PARAM:
	    size = sizeof(xsltStyleItemParam); break;
	case XSLT_FUNC_VARIABLE:
	    size = sizeof(xsltStyleItemVariable); break;
	case XSLT_FUNC_WHEN:
	    size = sizeof(xsltStyleItemWhen); break;
	case XSLT_FUNC_OTHERWISE:
	    size = sizeof(xsltStyleItemOtherwise); break;
	default:
	    xsltTransformError(NULL, style, NULL,
		    "xsltNewStylePreComp : invalid type %d\n", type);
	    style->errors++;
	    return(NULL);
    }
    /*
    * Create the structure.
    */
    cur = (xsltStylePreCompPtr) xmlMalloc(size);
    if (cur == NULL) {
	xsltTransformError(NULL, style, NULL,
		"xsltNewStylePreComp : malloc failed\n");
	style->errors++;
	return(NULL);
    }
    memset(cur, 0, size);

#else /* XSLT_REFACTORED */
    /*
    * Old behaviour.
    */
    cur = (xsltStylePreCompPtr) xmlMalloc(sizeof(xsltStylePreComp));
    if (cur == NULL) {
	xsltTransformError(NULL, style, NULL,
		"xsltNewStylePreComp : malloc failed\n");
	style->errors++;
	return(NULL);
    }
    memset(cur, 0, sizeof(xsltStylePreComp));
#endif /* XSLT_REFACTORED */

    /*
    * URGENT TODO: Better to move this to spezialized factory functions.
    */
    cur->type = type;
    switch (cur->type) {
        case XSLT_FUNC_COPY:
            cur->func = (xsltTransformFunction) xsltCopy;break;
        case XSLT_FUNC_SORT:
            cur->func = (xsltTransformFunction) xsltSort;break;
        case XSLT_FUNC_TEXT:
            cur->func = (xsltTransformFunction) xsltText;break;
        case XSLT_FUNC_ELEMENT:
            cur->func = (xsltTransformFunction) xsltElement;break;
        case XSLT_FUNC_ATTRIBUTE:
            cur->func = (xsltTransformFunction) xsltAttribute;break;
        case XSLT_FUNC_COMMENT:
            cur->func = (xsltTransformFunction) xsltComment;break;
        case XSLT_FUNC_PI:
            cur->func = (xsltTransformFunction) xsltProcessingInstruction;
	    break;
        case XSLT_FUNC_COPYOF:
            cur->func = (xsltTransformFunction) xsltCopyOf;break;
        case XSLT_FUNC_VALUEOF:
            cur->func = (xsltTransformFunction) xsltValueOf;break;
        case XSLT_FUNC_NUMBER:
            cur->func = (xsltTransformFunction) xsltNumber;break;
        case XSLT_FUNC_APPLYIMPORTS:
            cur->func = (xsltTransformFunction) xsltApplyImports;break;
        case XSLT_FUNC_CALLTEMPLATE:
            cur->func = (xsltTransformFunction) xsltCallTemplate;break;
        case XSLT_FUNC_APPLYTEMPLATES:
            cur->func = (xsltTransformFunction) xsltApplyTemplates;break;
        case XSLT_FUNC_CHOOSE:
            cur->func = (xsltTransformFunction) xsltChoose;break;
        case XSLT_FUNC_IF:
            cur->func = (xsltTransformFunction) xsltIf;break;
        case XSLT_FUNC_FOREACH:
            cur->func = (xsltTransformFunction) xsltForEach;break;
        case XSLT_FUNC_DOCUMENT:
            cur->func = (xsltTransformFunction) xsltDocumentElem;break;
	case XSLT_FUNC_WITHPARAM:
	case XSLT_FUNC_PARAM:
	case XSLT_FUNC_VARIABLE:
	case XSLT_FUNC_WHEN:
	    break;
	default:
	if (cur->func == NULL) {
	    xsltTransformError(NULL, style, NULL,
		    "xsltNewStylePreComp : no function for type %d\n", type);
	    style->errors++;
	}
    }
    cur->next = style->preComps;
    style->preComps = (xsltElemPreCompPtr) cur;

    return(cur);
}