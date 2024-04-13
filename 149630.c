is_full_opt_exact_info(OptExactInfo* ex)
{
  return (ex->len >= OPT_EXACT_MAXLEN ? 1 : 0);
}