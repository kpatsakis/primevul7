my_bool STDCALL mysql_stmt_attr_get(MYSQL_STMT *stmt,
                                    enum enum_stmt_attr_type attr_type,
                                    void *value)
{
  switch (attr_type) {
  case STMT_ATTR_UPDATE_MAX_LENGTH:
    *(my_bool*) value= stmt->update_max_length;
    break;
  case STMT_ATTR_CURSOR_TYPE:
    *(ulong*) value= stmt->flags;
    break;
  case STMT_ATTR_PREFETCH_ROWS:
    *(ulong*) value= stmt->prefetch_rows;
    break;
  default:
    return TRUE;
  }
  return FALSE;
}