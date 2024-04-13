mysql_stmt_result_metadata(MYSQL_STMT *stmt)
{
  MYSQL_RES *result;
  DBUG_ENTER("mysql_stmt_result_metadata");

  /*
    stmt->fields is only defined if stmt->field_count is not null;
    stmt->field_count is initialized in prepare.
  */
  if (!stmt->field_count)
     DBUG_RETURN(0);

  if (!(result=(MYSQL_RES*) my_malloc(sizeof(*result),
                                      MYF(MY_WME | MY_ZEROFILL))))
  {
    set_stmt_error(stmt, CR_OUT_OF_MEMORY, unknown_sqlstate, NULL);
    DBUG_RETURN(0);
  }

  result->methods=	stmt->mysql->methods;
  result->eof=		1;                      /* Marker for buffered */
  result->fields=	stmt->fields;
  result->field_count=	stmt->field_count;
  /* The rest of members of 'result' was bzeroed inside malloc */
  DBUG_RETURN(result);
}