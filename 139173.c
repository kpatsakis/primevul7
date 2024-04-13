int cli_read_binary_rows(MYSQL_STMT *stmt)
{
  ulong      pkt_len;
  uchar      *cp;
  MYSQL      *mysql= stmt->mysql;
  MYSQL_DATA *result= &stmt->result;
  MYSQL_ROWS *cur, **prev_ptr= &result->data;
  NET        *net;

  DBUG_ENTER("cli_read_binary_rows");

  if (!mysql)
  {
    set_stmt_error(stmt, CR_SERVER_LOST, unknown_sqlstate, NULL);
    DBUG_RETURN(1);
  }

  net = &mysql->net;

  while ((pkt_len= cli_safe_read(mysql)) != packet_error)
  {
    cp= net->read_pos;
    if (cp[0] != 254 || pkt_len >= 8)
    {
      if (!(cur= (MYSQL_ROWS*) alloc_root(&result->alloc,
                                          sizeof(MYSQL_ROWS) + pkt_len - 1)))
      {
        set_stmt_error(stmt, CR_OUT_OF_MEMORY, unknown_sqlstate, NULL);
        goto err;
      }
      cur->data= (MYSQL_ROW) (cur+1);
      *prev_ptr= cur;
      prev_ptr= &cur->next;
      memcpy((char *) cur->data, (char *) cp+1, pkt_len-1);
      cur->length= pkt_len;		/* To allow us to do sanity checks */
      result->rows++;
    }
    else
    {
      /* end of data */
      *prev_ptr= 0;
      mysql->warning_count= uint2korr(cp+1);
      mysql->server_status= uint2korr(cp+3);
      DBUG_PRINT("info",("status: %u  warning_count: %u",
                         mysql->server_status, mysql->warning_count));
      DBUG_RETURN(0);
    }
  }
  set_stmt_errmsg(stmt, net);

err:
  DBUG_RETURN(1);
}