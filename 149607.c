onigenc_minimum_property_name_to_ctype(OnigEncoding enc, const UChar* p, const UChar* end)
{
  static const PosixBracketEntryType PBS[] = {
    POSIX_BRACKET_ENTRY_INIT("Alnum",  ONIGENC_CTYPE_ALNUM),
    POSIX_BRACKET_ENTRY_INIT("Alpha",  ONIGENC_CTYPE_ALPHA),
    POSIX_BRACKET_ENTRY_INIT("Blank",  ONIGENC_CTYPE_BLANK),
    POSIX_BRACKET_ENTRY_INIT("Cntrl",  ONIGENC_CTYPE_CNTRL),
    POSIX_BRACKET_ENTRY_INIT("Digit",  ONIGENC_CTYPE_DIGIT),
    POSIX_BRACKET_ENTRY_INIT("Graph",  ONIGENC_CTYPE_GRAPH),
    POSIX_BRACKET_ENTRY_INIT("Lower",  ONIGENC_CTYPE_LOWER),
    POSIX_BRACKET_ENTRY_INIT("Print",  ONIGENC_CTYPE_PRINT),
    POSIX_BRACKET_ENTRY_INIT("Punct",  ONIGENC_CTYPE_PUNCT),
    POSIX_BRACKET_ENTRY_INIT("Space",  ONIGENC_CTYPE_SPACE),
    POSIX_BRACKET_ENTRY_INIT("Upper",  ONIGENC_CTYPE_UPPER),
    POSIX_BRACKET_ENTRY_INIT("XDigit", ONIGENC_CTYPE_XDIGIT),
    POSIX_BRACKET_ENTRY_INIT("ASCII",  ONIGENC_CTYPE_ASCII),
    POSIX_BRACKET_ENTRY_INIT("Word",   ONIGENC_CTYPE_WORD),
  };

  const PosixBracketEntryType *pb;
  int len;

  len = onigenc_strlen(enc, p, end);
  for (pb = PBS; pb < PBS + numberof(PBS); pb++) {
    if (len == pb->len &&
        onigenc_with_ascii_strnicmp(enc, p, end, pb->name, pb->len) == 0)
      return pb->ctype;
  }

  return ONIGERR_INVALID_CHAR_PROPERTY_NAME;
}