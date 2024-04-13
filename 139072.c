static my_bool store_param(MYSQL_STMT *stmt, MYSQL_BIND *param)
{
  NET *net= &stmt->mysql->net;
  DBUG_ENTER("store_param");
  DBUG_PRINT("enter",("type: %d  buffer: 0x%lx  length: %lu  is_null: %d",
		      param->buffer_type,
		      (long) (param->buffer ? param->buffer : NullS),
                      *param->length, *param->is_null));

  if (*param->is_null)
    store_param_null(net, param);
  else
  {
    /*
      Param->length should ALWAYS point to the correct length for the type
      Either to the length pointer given by the user or param->buffer_length
    */
    if ((my_realloc_str(net, *param->length)))
    {
      set_stmt_errmsg(stmt, net);
      DBUG_RETURN(1);
    }
    (*param->store_param_func)(net, param);
  }
  DBUG_RETURN(0);
}