onig_noname_group_capture_is_active(const regex_t* reg)
{
  if (ONIG_IS_OPTION_ON(reg->options, ONIG_OPTION_DONT_CAPTURE_GROUP))
    return 0;

#ifdef USE_NAMED_GROUP
  if (onig_number_of_names(reg) > 0 &&
      IS_SYNTAX_BV(reg->syntax, ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP) &&
      !ONIG_IS_OPTION_ON(reg->options, ONIG_OPTION_CAPTURE_GROUP)) {
    return 0;
  }
#endif

  return 1;
}