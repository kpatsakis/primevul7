my_bool STDCALL mysql_more_results(MYSQL *mysql)
{
  my_bool res;
  DBUG_ENTER("mysql_more_results");

  res= ((mysql->server_status & SERVER_MORE_RESULTS_EXISTS) ? 1: 0);
  DBUG_PRINT("exit",("More results exists ? %d", res));
  DBUG_RETURN(res);
}