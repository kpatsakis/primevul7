onig_strcpy(UChar* dest, const UChar* src, const UChar* end)
{
  ptrdiff_t len = end - src;
  if (len > 0) {
    xmemcpy(dest, src, len);
    dest[len] = (UChar )0;
  }
}