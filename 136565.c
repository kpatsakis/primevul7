XML_SetHashSalt(XML_Parser parser,
                unsigned long hash_salt)
{
  /* block after XML_Parse()/XML_ParseBuffer() has been called */
  if (ps_parsing == XML_PARSING || ps_parsing == XML_SUSPENDED)
    return 0;
  hash_secret_salt = hash_salt;
  return 1;
}