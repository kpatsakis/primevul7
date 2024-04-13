extern int onig_set_meta_char(OnigSyntaxType* enc,
                              unsigned int what, OnigCodePoint code)
{
  switch (what) {
  case ONIG_META_CHAR_ESCAPE:
    enc->meta_char_table.esc = code;
    break;
  case ONIG_META_CHAR_ANYCHAR:
    enc->meta_char_table.anychar = code;
    break;
  case ONIG_META_CHAR_ANYTIME:
    enc->meta_char_table.anytime = code;
    break;
  case ONIG_META_CHAR_ZERO_OR_ONE_TIME:
    enc->meta_char_table.zero_or_one_time = code;
    break;
  case ONIG_META_CHAR_ONE_OR_MORE_TIME:
    enc->meta_char_table.one_or_more_time = code;
    break;
  case ONIG_META_CHAR_ANYCHAR_ANYTIME:
    enc->meta_char_table.anychar_anytime = code;
    break;
  default:
    return ONIGERR_INVALID_ARGUMENT;
    break;
  }
  return 0;
}