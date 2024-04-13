xsltStylePreCompute(xsltStylesheetPtr style, xmlNodePtr node) {
    /*
    * The xsltXSLTElemMarker marker was set beforehand by
    *  the parsing mechanism for all elements in the XSLT namespace.
    */
    if (style == NULL) {
	if ((node != NULL) && (node->type == XML_ELEMENT_NODE))
	    node->psvi = NULL;
	return;
    }
    if (node == NULL)
	return;
    if (! IS_XSLT_ELEM_FAST(node))
	return;

    node->psvi = NULL;
    if (XSLT_CCTXT(style)->inode->type != 0) {
	switch (XSLT_CCTXT(style)->inode->type) {
	    case XSLT_FUNC_APPLYTEMPLATES:
		xsltApplyTemplatesComp(style, node);
		break;
	    case XSLT_FUNC_WITHPARAM:
		xsltWithParamComp(style, node);
		break;
	    case XSLT_FUNC_VALUEOF:
		xsltValueOfComp(style, node);
		break;
	    case XSLT_FUNC_COPY:
		xsltCopyComp(style, node);
		break;
	    case XSLT_FUNC_COPYOF:
		xsltCopyOfComp(style, node);
		break;
	    case XSLT_FUNC_IF:
		xsltIfComp(style, node);
		break;
	    case XSLT_FUNC_CHOOSE:
		xsltChooseComp(style, node);
		break;
	    case XSLT_FUNC_WHEN:
		xsltWhenComp(style, node);
		break;
	    case XSLT_FUNC_OTHERWISE:
		/* NOP yet */
		return;
	    case XSLT_FUNC_FOREACH:
		xsltForEachComp(style, node);
		break;
	    case XSLT_FUNC_APPLYIMPORTS:
		xsltApplyImportsComp(style, node);
		break;
	    case XSLT_FUNC_ATTRIBUTE:
		xsltAttributeComp(style, node);
		break;
	    case XSLT_FUNC_ELEMENT:
		xsltElementComp(style, node);
		break;
	    case XSLT_FUNC_SORT:
		xsltSortComp(style, node);
		break;
	    case XSLT_FUNC_COMMENT:
		xsltCommentComp(style, node);
		break;
	    case XSLT_FUNC_NUMBER:
		xsltNumberComp(style, node);
		break;
	    case XSLT_FUNC_PI:
		xsltProcessingInstructionComp(style, node);
		break;
	    case XSLT_FUNC_CALLTEMPLATE:
		xsltCallTemplateComp(style, node);
		break;
	    case XSLT_FUNC_PARAM:
		xsltParamComp(style, node);
		break;
	    case XSLT_FUNC_VARIABLE:
		xsltVariableComp(style, node);
		break;
	    case XSLT_FUNC_FALLBACK:
		/* NOP yet */
		return;
	    case XSLT_FUNC_DOCUMENT:
		/* The extra one */
		node->psvi = (void *) xsltDocumentComp(style, node,
		    (xsltTransformFunction) xsltDocumentElem);
		break;
	    case XSLT_FUNC_MESSAGE:
		/* NOP yet */
		return;
	    default:
		/*
		* NOTE that xsl:text, xsl:template, xsl:stylesheet,
		*  xsl:transform, xsl:import, xsl:include are not expected
		*  to be handed over to this function.
		*/
		xsltTransformError(NULL, style, node,
		    "Internal error: (xsltStylePreCompute) cannot handle "
		    "the XSLT element '%s'.\n", node->name);
		style->errors++;
		return;
	}
    } else {
	/*
	* Fallback to string comparison.
	*/
	if (IS_XSLT_NAME(node, "apply-templates")) {
	    xsltApplyTemplatesComp(style, node);
	} else if (IS_XSLT_NAME(node, "with-param")) {
	    xsltWithParamComp(style, node);
	} else if (IS_XSLT_NAME(node, "value-of")) {
	    xsltValueOfComp(style, node);
	} else if (IS_XSLT_NAME(node, "copy")) {
	    xsltCopyComp(style, node);
	} else if (IS_XSLT_NAME(node, "copy-of")) {
	    xsltCopyOfComp(style, node);
	} else if (IS_XSLT_NAME(node, "if")) {
	    xsltIfComp(style, node);
	} else if (IS_XSLT_NAME(node, "choose")) {
	    xsltChooseComp(style, node);
	} else if (IS_XSLT_NAME(node, "when")) {
	    xsltWhenComp(style, node);
	} else if (IS_XSLT_NAME(node, "otherwise")) {
	    /* NOP yet */
	    return;
	} else if (IS_XSLT_NAME(node, "for-each")) {
	    xsltForEachComp(style, node);
	} else if (IS_XSLT_NAME(node, "apply-imports")) {
	    xsltApplyImportsComp(style, node);
	} else if (IS_XSLT_NAME(node, "attribute")) {
	    xsltAttributeComp(style, node);
	} else if (IS_XSLT_NAME(node, "element")) {
	    xsltElementComp(style, node);
	} else if (IS_XSLT_NAME(node, "sort")) {
	    xsltSortComp(style, node);
	} else if (IS_XSLT_NAME(node, "comment")) {
	    xsltCommentComp(style, node);
	} else if (IS_XSLT_NAME(node, "number")) {
	    xsltNumberComp(style, node);
	} else if (IS_XSLT_NAME(node, "processing-instruction")) {
	    xsltProcessingInstructionComp(style, node);
	} else if (IS_XSLT_NAME(node, "call-template")) {
	    xsltCallTemplateComp(style, node);
	} else if (IS_XSLT_NAME(node, "param")) {
	    xsltParamComp(style, node);
	} else if (IS_XSLT_NAME(node, "variable")) {
	    xsltVariableComp(style, node);
	} else if (IS_XSLT_NAME(node, "fallback")) {
	    /* NOP yet */
	    return;
	} else if (IS_XSLT_NAME(node, "document")) {
	    /* The extra one */
	    node->psvi = (void *) xsltDocumentComp(style, node,
		(xsltTransformFunction) xsltDocumentElem);
	} else if (IS_XSLT_NAME(node, "output")) {
	    /* Top-level */
	    return;
	} else if (IS_XSLT_NAME(node, "preserve-space")) {
	    /* Top-level */
	    return;
	} else if (IS_XSLT_NAME(node, "strip-space")) {
	    /* Top-level */
	    return;
	} else if (IS_XSLT_NAME(node, "key")) {
	    /* Top-level */
	    return;
	} else if (IS_XSLT_NAME(node, "message")) {
	    return;
	} else if (IS_XSLT_NAME(node, "attribute-set")) {
	    /* Top-level */
	    return;
	} else if (IS_XSLT_NAME(node, "namespace-alias")) {
	    /* Top-level */
	    return;
	} else if (IS_XSLT_NAME(node, "decimal-format")) {
	    /* Top-level */
	    return;
	} else if (IS_XSLT_NAME(node, "include")) {
	    /* Top-level */
	} else {
	    /*
	    * NOTE that xsl:text, xsl:template, xsl:stylesheet,
	    *  xsl:transform, xsl:import, xsl:include are not expected
	    *  to be handed over to this function.
	    */
	    xsltTransformError(NULL, style, node,
		"Internal error: (xsltStylePreCompute) cannot handle "
		"the XSLT element '%s'.\n", node->name);
		style->errors++;
	    return;
	}
    }
    /*
    * Assign the current list of in-scope namespaces to the
    * item. This is needed for XPath expressions.
    */
    if (node->psvi != NULL) {
	((xsltStylePreCompPtr) node->psvi)->inScopeNs =
	    XSLT_CCTXT(style)->inode->inScopeNs;
    }
}