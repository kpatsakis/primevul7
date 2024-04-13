stmt_read_row_no_result_set(MYSQL_STMT *stmt  __attribute__((unused)),
                      unsigned char **row  __attribute__((unused)))
{
  set_stmt_error(stmt, CR_NO_RESULT_SET, unknown_sqlstate, NULL);
  return 1;
}