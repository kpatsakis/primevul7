XML_ParserFree(XML_Parser parser)
{
  TAG *tagList;
  OPEN_INTERNAL_ENTITY *entityList;
  if (parser == NULL)
    return;
  /* free tagStack and freeTagList */
  tagList = tagStack;
  for (;;) {
    TAG *p;
    if (tagList == NULL) {
      if (freeTagList == NULL)
        break;
      tagList = freeTagList;
      freeTagList = NULL;
    }
    p = tagList;
    tagList = tagList->parent;
    FREE(p->buf);
    destroyBindings(p->bindings, parser);
    FREE(p);
  }
  /* free openInternalEntities and freeInternalEntities */
  entityList = openInternalEntities;
  for (;;) {
    OPEN_INTERNAL_ENTITY *openEntity;
    if (entityList == NULL) {
      if (freeInternalEntities == NULL)
        break;
      entityList = freeInternalEntities;
      freeInternalEntities = NULL;
    }
    openEntity = entityList;
    entityList = entityList->next;
    FREE(openEntity);
  }

  destroyBindings(freeBindingList, parser);
  destroyBindings(inheritedBindings, parser);
  poolDestroy(&tempPool);
  poolDestroy(&temp2Pool);
#ifdef XML_DTD
  /* external parameter entity parsers share the DTD structure
     parser->m_dtd with the root parser, so we must not destroy it
  */
  if (!isParamEntity && _dtd)
#else
  if (_dtd)
#endif /* XML_DTD */
    dtdDestroy(_dtd, (XML_Bool)!parentParser, &parser->m_mem);
  FREE((void *)atts);
#ifdef XML_ATTR_INFO
  FREE((void *)attInfo);
#endif
  FREE(groupConnector);
  FREE(buffer);
  FREE(dataBuf);
  FREE(nsAtts);
  FREE(unknownEncodingMem);
  if (unknownEncodingRelease)
    unknownEncodingRelease(unknownEncodingData);
  FREE(parser);
}