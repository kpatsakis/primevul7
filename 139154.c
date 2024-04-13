int cli_unbuffered_fetch(MYSQL *mysql, char **row)
{
  if (packet_error == cli_safe_read(mysql))
    return 1;

  *row= ((mysql->net.read_pos[0] == 254) ? NULL :
	 (char*) (mysql->net.read_pos+1));
  return 0;
}