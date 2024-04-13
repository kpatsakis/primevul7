int cli_stmt_execute(MYSQL_STMT *stmt)
{
  DBUG_ENTER("cli_stmt_execute");

  if (stmt->param_count)
  {
    MYSQL *mysql= stmt->mysql;
    NET        *net= &mysql->net;
    MYSQL_BIND *param, *param_end;
    char       *param_data;
    ulong length;
    uint null_count;
    my_bool    result;

    if (!stmt->bind_param_done)
    {
      set_stmt_error(stmt, CR_PARAMS_NOT_BOUND, unknown_sqlstate, NULL);
      DBUG_RETURN(1);
    }
    if (mysql->status != MYSQL_STATUS_READY ||
        mysql->server_status & SERVER_MORE_RESULTS_EXISTS)
    {
      set_stmt_error(stmt, CR_COMMANDS_OUT_OF_SYNC, unknown_sqlstate, NULL);
      DBUG_RETURN(1);
    }

    if (net->vio)
      net_clear(net, 1);          /* Sets net->write_pos */
    else
    {
      set_stmt_errmsg(stmt, net);
      DBUG_RETURN(1);             
    }

    /* Reserve place for null-marker bytes */
    null_count= (stmt->param_count+7) /8;
    if (my_realloc_str(net, null_count + 1))
    {
      set_stmt_errmsg(stmt, net);
      DBUG_RETURN(1);
    }
    bzero((char*) net->write_pos, null_count);
    net->write_pos+= null_count;
    param_end= stmt->params + stmt->param_count;

    /* In case if buffers (type) altered, indicate to server */
    *(net->write_pos)++= (uchar) stmt->send_types_to_server;
    if (stmt->send_types_to_server)
    {
      if (my_realloc_str(net, 2 * stmt->param_count))
      {
        set_stmt_errmsg(stmt, net);
        DBUG_RETURN(1);
      }
      /*
	Store types of parameters in first in first package
	that is sent to the server.
      */
      for (param= stmt->params;	param < param_end ; param++)
        store_param_type(&net->write_pos, param);
    }

    for (param= stmt->params; param < param_end; param++)
    {
      /* check if mysql_stmt_send_long_data() was used */
      if (param->long_data_used)
	param->long_data_used= 0;	/* Clear for next execute call */
      else if (store_param(stmt, param))
	DBUG_RETURN(1);
    }
    length= (ulong) (net->write_pos - net->buff);
    /* TODO: Look into avoding the following memdup */
    if (!(param_data= my_memdup(net->buff, length, MYF(0))))
    {
      set_stmt_error(stmt, CR_OUT_OF_MEMORY, unknown_sqlstate, NULL);
      DBUG_RETURN(1);
    }
    result= execute(stmt, param_data, length);
    stmt->send_types_to_server=0;
    my_free(param_data);
    DBUG_RETURN(result);
  }
  DBUG_RETURN((int) execute(stmt,0,0));
}