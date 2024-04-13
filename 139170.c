my_ulonglong STDCALL mysql_stmt_insert_id(MYSQL_STMT *stmt)
{
  return stmt->insert_id;
}