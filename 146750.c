static xmlParserInputPtr SVGResolveEntity(void *context,
  const xmlChar *public_id,const xmlChar *system_id)
{
  SVGInfo
    *svg_info;

  xmlParserInputPtr
    stream;

  /*
    Special entity resolver, better left to the parser, it has more
    context than the application layer.  The default behaviour is to
    not resolve the entities, in that case the ENTITY_REF nodes are
    built in the structure (and the parameter values).
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  SAX.resolveEntity(%s, %s)",
    (public_id != (const xmlChar *) NULL ? (const char *) public_id : "none"),
    (system_id != (const xmlChar *) NULL ? (const char *) system_id : "none"));
  svg_info=(SVGInfo *) context;
  stream=xmlLoadExternalEntity((const char *) system_id,(const char *)
    public_id,svg_info->parser);
  return(stream);
}