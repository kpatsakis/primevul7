mysql_real_escape_string(MYSQL *mysql, char *to,const char *from,
			 ulong length)
{
  if (mysql->server_status & SERVER_STATUS_NO_BACKSLASH_ESCAPES)
    return (uint) escape_quotes_for_mysql(mysql->charset, to, 0, from, length);
  return (uint) escape_string_for_mysql(mysql->charset, to, 0, from, length);
}