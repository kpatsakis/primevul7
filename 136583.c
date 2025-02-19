startParsing(XML_Parser parser)
{
    /* hash functions must be initialized before setContext() is called */
    if (hash_secret_salt == 0)
      hash_secret_salt = generate_hash_secret_salt();
    if (ns) {
      /* implicit context only set for root parser, since child
         parsers (i.e. external entity parsers) will inherit it
      */
      return setContext(parser, implicitContext);
    }
    return XML_TRUE;
}