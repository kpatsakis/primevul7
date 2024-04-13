hash(XML_Parser parser, KEY s)
{
  unsigned long h = hash_secret_salt;
  while (*s)
    h = CHAR_HASH(h, *s++);
  return h;
}