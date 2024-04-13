static int exec(FILE* fp, ENC_INFO* einfo)
{
#define NCOL  8

  int c, val, enc;
  int r;

  enc = einfo->num;

  r = fprintf(fp, "static const unsigned short Enc%s_CtypeTable[256] = {\n",
              einfo->name);
  if (r < 0) return -1;

  for (c = 0; c < 256; c++) {
    val = 0;
    if (IsNewline(enc, c))  val |= BIT_CTYPE_NEWLINE;
    if (IsAlpha (enc, c))   val |= (BIT_CTYPE_ALPHA | BIT_CTYPE_ALNUM);
    if (IsBlank (enc, c))   val |= BIT_CTYPE_BLANK;
    if (IsCntrl (enc, c))   val |= BIT_CTYPE_CNTRL;
    if (IsDigit (enc, c))   val |= (BIT_CTYPE_DIGIT | BIT_CTYPE_ALNUM);
    if (IsGraph (enc, c))   val |= BIT_CTYPE_GRAPH;
    if (IsLower (enc, c))   val |= BIT_CTYPE_LOWER;
    if (IsPrint (enc, c))   val |= BIT_CTYPE_PRINT;
    if (IsPunct (enc, c))   val |= BIT_CTYPE_PUNCT;
    if (IsSpace (enc, c))   val |= BIT_CTYPE_SPACE;
    if (IsUpper (enc, c))   val |= BIT_CTYPE_UPPER;
    if (IsXDigit(enc, c))   val |= BIT_CTYPE_XDIGIT;
    if (IsWord  (enc, c))   val |= BIT_CTYPE_WORD;
    if (IsAscii (enc, c))   val |= BIT_CTYPE_ASCII;

    if (c % NCOL == 0) {
      r = fputs("  ", fp);
      if (r < 0) return -1;
    }
    r = fprintf(fp, "0x%04x", val);
    if (r < 0) return -1;

    if (c != 255) {
      r = fputs(",", fp);
      if (r < 0) return -1;
    }
    if (c != 0 && c % NCOL == (NCOL-1))
      r = fputs("\n", fp);
    else
      r = fputs(" ", fp);

    if (r < 0) return -1;
  }
  r = fprintf(fp, "};\n");
  if (r < 0) return -1;

  return 0;
}