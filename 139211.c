static void prepare_to_fetch_result(MYSQL_STMT *stmt)
{
  if (stmt->server_status & SERVER_STATUS_CURSOR_EXISTS)
  {
    stmt->mysql->status= MYSQL_STATUS_READY;
    stmt->read_row_func= stmt_read_row_from_cursor;
  }
  else if (stmt->flags & CURSOR_TYPE_READ_ONLY)
  {
    /*
      This is a single-row result set, a result set with no rows, EXPLAIN,
      SHOW VARIABLES, or some other command which either a) bypasses the
      cursors framework in the server and writes rows directly to the
      network or b) is more efficient if all (few) result set rows are
      precached on client and server's resources are freed.
    */
    mysql_stmt_store_result(stmt);
  }
  else
  {
    stmt->mysql->unbuffered_fetch_owner= &stmt->unbuffered_fetch_cancelled;
    stmt->unbuffered_fetch_cancelled= FALSE;
    stmt->read_row_func= stmt_read_row_unbuffered;
  }
}