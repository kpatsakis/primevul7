my_bool STDCALL mysql_autocommit(MYSQL * mysql, my_bool auto_mode)
{
  DBUG_ENTER("mysql_autocommit");
  DBUG_PRINT("enter", ("mode : %d", auto_mode));

  DBUG_RETURN((my_bool) mysql_real_query(mysql, auto_mode ?
                                         "set autocommit=1":"set autocommit=0",
                                         16));
}