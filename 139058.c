my_bool STDCALL mysql_eof(MYSQL_RES *res)
{
  return res->eof;
}