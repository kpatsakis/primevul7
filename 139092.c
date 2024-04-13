stmt_read_row_no_data(MYSQL_STMT *stmt  __attribute__((unused)),
                      unsigned char **row  __attribute__((unused)))
{
  return MYSQL_NO_DATA;
}