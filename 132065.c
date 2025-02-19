xmlParseReference(xmlParserCtxtPtr ctxt) {
    xmlEntityPtr ent;
    xmlChar *val;
    int was_checked;
    xmlNodePtr list = NULL;
    xmlParserErrors ret = XML_ERR_OK;


    if (RAW != '&')
        return;

    /*
     * Simple case of a CharRef
     */
    if (NXT(1) == '#') {
	int i = 0;
	xmlChar out[10];
	int hex = NXT(2);
	int value = xmlParseCharRef(ctxt);

	if (value == 0)
	    return;
	if (ctxt->charset != XML_CHAR_ENCODING_UTF8) {
	    /*
	     * So we are using non-UTF-8 buffers
	     * Check that the char fit on 8bits, if not
	     * generate a CharRef.
	     */
	    if (value <= 0xFF) {
		out[0] = value;
		out[1] = 0;
		if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL) &&
		    (!ctxt->disableSAX))
		    ctxt->sax->characters(ctxt->userData, out, 1);
	    } else {
		if ((hex == 'x') || (hex == 'X'))
		    snprintf((char *)out, sizeof(out), "#x%X", value);
		else
		    snprintf((char *)out, sizeof(out), "#%d", value);
		if ((ctxt->sax != NULL) && (ctxt->sax->reference != NULL) &&
		    (!ctxt->disableSAX))
		    ctxt->sax->reference(ctxt->userData, out);
	    }
	} else {
	    /*
	     * Just encode the value in UTF-8
	     */
	    COPY_BUF(0 ,out, i, value);
	    out[i] = 0;
	    if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL) &&
		(!ctxt->disableSAX))
		ctxt->sax->characters(ctxt->userData, out, i);
	}
	return;
    }

    /*
     * We are seeing an entity reference
     */
    ent = xmlParseEntityRef(ctxt);
    if (ent == NULL) return;
    if (!ctxt->wellFormed)
	return;
    was_checked = ent->checked;

    /* special case of predefined entities */
    if ((ent->name == NULL) ||
        (ent->etype == XML_INTERNAL_PREDEFINED_ENTITY)) {
	val = ent->content;
	if (val == NULL) return;
	/*
	 * inline the entity.
	 */
	if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL) &&
	    (!ctxt->disableSAX))
	    ctxt->sax->characters(ctxt->userData, val, xmlStrlen(val));
	return;
    }

    /*
     * The first reference to the entity trigger a parsing phase
     * where the ent->children is filled with the result from
     * the parsing.
     * Note: external parsed entities will not be loaded, it is not
     * required for a non-validating parser, unless the parsing option
     * of validating, or substituting entities were given. Doing so is
     * far more secure as the parser will only process data coming from
     * the document entity by default.
     */
    if (((ent->checked == 0) ||
         ((ent->children == NULL) && (ctxt->options & XML_PARSE_NOENT))) &&
        ((ent->etype != XML_EXTERNAL_GENERAL_PARSED_ENTITY) ||
         (ctxt->options & (XML_PARSE_NOENT | XML_PARSE_DTDVALID)))) {
	unsigned long oldnbent = ctxt->nbentities;

	/*
	 * This is a bit hackish but this seems the best
	 * way to make sure both SAX and DOM entity support
	 * behaves okay.
	 */
	void *user_data;
	if (ctxt->userData == ctxt)
	    user_data = NULL;
	else
	    user_data = ctxt->userData;

	/*
	 * Check that this entity is well formed
	 * 4.3.2: An internal general parsed entity is well-formed
	 * if its replacement text matches the production labeled
	 * content.
	 */
	if (ent->etype == XML_INTERNAL_GENERAL_ENTITY) {
	    ctxt->depth++;
	    ret = xmlParseBalancedChunkMemoryInternal(ctxt, ent->content,
	                                              user_data, &list);
	    ctxt->depth--;

	} else if (ent->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY) {
	    ctxt->depth++;
	    ret = xmlParseExternalEntityPrivate(ctxt->myDoc, ctxt, ctxt->sax,
	                                   user_data, ctxt->depth, ent->URI,
					   ent->ExternalID, &list);
	    ctxt->depth--;
	} else {
	    ret = XML_ERR_ENTITY_PE_INTERNAL;
	    xmlErrMsgStr(ctxt, XML_ERR_INTERNAL_ERROR,
			 "invalid entity type found\n", NULL);
	}

	/*
	 * Store the number of entities needing parsing for this entity
	 * content and do checkings
	 */
	ent->checked = (ctxt->nbentities - oldnbent + 1) * 2;
	if ((ent->content != NULL) && (xmlStrchr(ent->content, '<')))
	    ent->checked |= 1;
	if (ret == XML_ERR_ENTITY_LOOP) {
	    xmlFatalErr(ctxt, XML_ERR_ENTITY_LOOP, NULL);
	    xmlFreeNodeList(list);
	    return;
	}
	if (xmlParserEntityCheck(ctxt, 0, ent, 0)) {
	    xmlFreeNodeList(list);
	    return;
	}

	if ((ret == XML_ERR_OK) && (list != NULL)) {
	    if (((ent->etype == XML_INTERNAL_GENERAL_ENTITY) ||
	     (ent->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY))&&
		(ent->children == NULL)) {
		ent->children = list;
		if (ctxt->replaceEntities) {
		    /*
		     * Prune it directly in the generated document
		     * except for single text nodes.
		     */
		    if (((list->type == XML_TEXT_NODE) &&
			 (list->next == NULL)) ||
			(ctxt->parseMode == XML_PARSE_READER)) {
			list->parent = (xmlNodePtr) ent;
			list = NULL;
			ent->owner = 1;
		    } else {
			ent->owner = 0;
			while (list != NULL) {
			    list->parent = (xmlNodePtr) ctxt->node;
			    list->doc = ctxt->myDoc;
			    if (list->next == NULL)
				ent->last = list;
			    list = list->next;
			}
			list = ent->children;
#ifdef LIBXML_LEGACY_ENABLED
			if (ent->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY)
			  xmlAddEntityReference(ent, list, NULL);
#endif /* LIBXML_LEGACY_ENABLED */
		    }
		} else {
		    ent->owner = 1;
		    while (list != NULL) {
			list->parent = (xmlNodePtr) ent;
			xmlSetTreeDoc(list, ent->doc);
			if (list->next == NULL)
			    ent->last = list;
			list = list->next;
		    }
		}
	    } else {
		xmlFreeNodeList(list);
		list = NULL;
	    }
	} else if ((ret != XML_ERR_OK) &&
		   (ret != XML_WAR_UNDECLARED_ENTITY)) {
	    xmlFatalErrMsgStr(ctxt, XML_ERR_UNDECLARED_ENTITY,
		     "Entity '%s' failed to parse\n", ent->name);
	    xmlParserEntityCheck(ctxt, 0, ent, 0);
	} else if (list != NULL) {
	    xmlFreeNodeList(list);
	    list = NULL;
	}
	if (ent->checked == 0)
	    ent->checked = 2;
    } else if (ent->checked != 1) {
	ctxt->nbentities += ent->checked / 2;
    }

    /*
     * Now that the entity content has been gathered
     * provide it to the application, this can take different forms based
     * on the parsing modes.
     */
    if (ent->children == NULL) {
	/*
	 * Probably running in SAX mode and the callbacks don't
	 * build the entity content. So unless we already went
	 * though parsing for first checking go though the entity
	 * content to generate callbacks associated to the entity
	 */
	if (was_checked != 0) {
	    void *user_data;
	    /*
	     * This is a bit hackish but this seems the best
	     * way to make sure both SAX and DOM entity support
	     * behaves okay.
	     */
	    if (ctxt->userData == ctxt)
		user_data = NULL;
	    else
		user_data = ctxt->userData;

	    if (ent->etype == XML_INTERNAL_GENERAL_ENTITY) {
		ctxt->depth++;
		ret = xmlParseBalancedChunkMemoryInternal(ctxt,
				   ent->content, user_data, NULL);
		ctxt->depth--;
	    } else if (ent->etype ==
		       XML_EXTERNAL_GENERAL_PARSED_ENTITY) {
		ctxt->depth++;
		ret = xmlParseExternalEntityPrivate(ctxt->myDoc, ctxt,
			   ctxt->sax, user_data, ctxt->depth,
			   ent->URI, ent->ExternalID, NULL);
		ctxt->depth--;
	    } else {
		ret = XML_ERR_ENTITY_PE_INTERNAL;
		xmlErrMsgStr(ctxt, XML_ERR_INTERNAL_ERROR,
			     "invalid entity type found\n", NULL);
	    }
	    if (ret == XML_ERR_ENTITY_LOOP) {
		xmlFatalErr(ctxt, XML_ERR_ENTITY_LOOP, NULL);
		return;
	    }
	}
	if ((ctxt->sax != NULL) && (ctxt->sax->reference != NULL) &&
	    (ctxt->replaceEntities == 0) && (!ctxt->disableSAX)) {
	    /*
	     * Entity reference callback comes second, it's somewhat
	     * superfluous but a compatibility to historical behaviour
	     */
	    ctxt->sax->reference(ctxt->userData, ent->name);
	}
	return;
    }

    /*
     * If we didn't get any children for the entity being built
     */
    if ((ctxt->sax != NULL) && (ctxt->sax->reference != NULL) &&
	(ctxt->replaceEntities == 0) && (!ctxt->disableSAX)) {
	/*
	 * Create a node.
	 */
	ctxt->sax->reference(ctxt->userData, ent->name);
	return;
    }

    if ((ctxt->replaceEntities) || (ent->children == NULL))  {
	/*
	 * There is a problem on the handling of _private for entities
	 * (bug 155816): Should we copy the content of the field from
	 * the entity (possibly overwriting some value set by the user
	 * when a copy is created), should we leave it alone, or should
	 * we try to take care of different situations?  The problem
	 * is exacerbated by the usage of this field by the xmlReader.
	 * To fix this bug, we look at _private on the created node
	 * and, if it's NULL, we copy in whatever was in the entity.
	 * If it's not NULL we leave it alone.  This is somewhat of a
	 * hack - maybe we should have further tests to determine
	 * what to do.
	 */
	if ((ctxt->node != NULL) && (ent->children != NULL)) {
	    /*
	     * Seems we are generating the DOM content, do
	     * a simple tree copy for all references except the first
	     * In the first occurrence list contains the replacement.
	     */
	    if (((list == NULL) && (ent->owner == 0)) ||
		(ctxt->parseMode == XML_PARSE_READER)) {
		xmlNodePtr nw = NULL, cur, firstChild = NULL;

		/*
		 * We are copying here, make sure there is no abuse
		 */
		ctxt->sizeentcopy += ent->length + 5;
		if (xmlParserEntityCheck(ctxt, 0, ent, ctxt->sizeentcopy))
		    return;

		/*
		 * when operating on a reader, the entities definitions
		 * are always owning the entities subtree.
		if (ctxt->parseMode == XML_PARSE_READER)
		    ent->owner = 1;
		 */

		cur = ent->children;
		while (cur != NULL) {
		    nw = xmlDocCopyNode(cur, ctxt->myDoc, 1);
		    if (nw != NULL) {
			if (nw->_private == NULL)
			    nw->_private = cur->_private;
			if (firstChild == NULL){
			    firstChild = nw;
			}
			nw = xmlAddChild(ctxt->node, nw);
		    }
		    if (cur == ent->last) {
			/*
			 * needed to detect some strange empty
			 * node cases in the reader tests
			 */
			if ((ctxt->parseMode == XML_PARSE_READER) &&
			    (nw != NULL) &&
			    (nw->type == XML_ELEMENT_NODE) &&
			    (nw->children == NULL))
			    nw->extra = 1;

			break;
		    }
		    cur = cur->next;
		}
#ifdef LIBXML_LEGACY_ENABLED
		if (ent->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY)
		  xmlAddEntityReference(ent, firstChild, nw);
#endif /* LIBXML_LEGACY_ENABLED */
	    } else if ((list == NULL) || (ctxt->inputNr > 0)) {
		xmlNodePtr nw = NULL, cur, next, last,
			   firstChild = NULL;

		/*
		 * We are copying here, make sure there is no abuse
		 */
		ctxt->sizeentcopy += ent->length + 5;
		if (xmlParserEntityCheck(ctxt, 0, ent, ctxt->sizeentcopy))
		    return;

		/*
		 * Copy the entity child list and make it the new
		 * entity child list. The goal is to make sure any
		 * ID or REF referenced will be the one from the
		 * document content and not the entity copy.
		 */
		cur = ent->children;
		ent->children = NULL;
		last = ent->last;
		ent->last = NULL;
		while (cur != NULL) {
		    next = cur->next;
		    cur->next = NULL;
		    cur->parent = NULL;
		    nw = xmlDocCopyNode(cur, ctxt->myDoc, 1);
		    if (nw != NULL) {
			if (nw->_private == NULL)
			    nw->_private = cur->_private;
			if (firstChild == NULL){
			    firstChild = cur;
			}
			xmlAddChild((xmlNodePtr) ent, nw);
			xmlAddChild(ctxt->node, cur);
		    }
		    if (cur == last)
			break;
		    cur = next;
		}
		if (ent->owner == 0)
		    ent->owner = 1;
#ifdef LIBXML_LEGACY_ENABLED
		if (ent->etype == XML_EXTERNAL_GENERAL_PARSED_ENTITY)
		  xmlAddEntityReference(ent, firstChild, nw);
#endif /* LIBXML_LEGACY_ENABLED */
	    } else {
		const xmlChar *nbktext;

		/*
		 * the name change is to avoid coalescing of the
		 * node with a possible previous text one which
		 * would make ent->children a dangling pointer
		 */
		nbktext = xmlDictLookup(ctxt->dict, BAD_CAST "nbktext",
					-1);
		if (ent->children->type == XML_TEXT_NODE)
		    ent->children->name = nbktext;
		if ((ent->last != ent->children) &&
		    (ent->last->type == XML_TEXT_NODE))
		    ent->last->name = nbktext;
		xmlAddChildList(ctxt->node, ent->children);
	    }

	    /*
	     * This is to avoid a nasty side effect, see
	     * characters() in SAX.c
	     */
	    ctxt->nodemem = 0;
	    ctxt->nodelen = 0;
	    return;
	}
    }
}