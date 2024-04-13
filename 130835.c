com_warnings(String *buffer __attribute__((unused)),
   char *line __attribute__((unused)))
{
  show_warnings = 1;
  put_info("Show warnings enabled.",INFO_INFO);
  return 0;
}