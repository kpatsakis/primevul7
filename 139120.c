static void reinit_result_set_metadata(MYSQL_STMT *stmt)
{
  /* Server has sent result set metadata */
  if (stmt->field_count == 0)
  {
    /*
      This is 'SHOW'/'EXPLAIN'-like query. Current implementation of
      prepared statements can't send result set metadata for these queries
      on prepare stage. Read it now.
    */

    stmt->field_count= stmt->mysql->field_count;

    alloc_stmt_fields(stmt);
  }
  else
  {
    /*
      Update result set metadata if it for some reason changed between
      prepare and execute, i.e.:
      - in case of 'SELECT ?' we don't know column type unless data was
      supplied to mysql_stmt_execute, so updated column type is sent
      now.
      - if data dictionary changed between prepare and execute, for
      example a table used in the query was altered.
      Note, that now (4.1.3) we always send metadata in reply to
      COM_STMT_EXECUTE (even if it is not necessary), so either this or
      previous branch always works.
      TODO: send metadata only when it's really necessary and add a warning
      'Metadata changed' when it's sent twice.
    */
    update_stmt_fields(stmt);
  }
}