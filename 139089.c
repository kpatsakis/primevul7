mysql_escape_string(char *to,const char *from,ulong length)
{
  return (uint) escape_string_for_mysql(default_charset_info, to, 0, from, length);
}