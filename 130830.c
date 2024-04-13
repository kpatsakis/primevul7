com_nowarnings(String *buffer __attribute__((unused)),
   char *line __attribute__((unused)))
{
  show_warnings = 0;
  put_info("Show warnings disabled.",INFO_INFO);
  return 0;
}