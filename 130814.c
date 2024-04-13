com_quit(String *buffer __attribute__((unused)),
	 char *line __attribute__((unused)))
{
  status.exit_status=0;
  return 1;
}