my_bool STDCALL mysql_stmt_attr_set(MYSQL_STMT *stmt,
                                    enum enum_stmt_attr_type attr_type,
                                    const void *value)
{
  switch (attr_type) {
  case STMT_ATTR_UPDATE_MAX_LENGTH:
    stmt->update_max_length= value ? *(const my_bool*) value : 0;
    break;
  case STMT_ATTR_CURSOR_TYPE:
  {
    ulong cursor_type;
    cursor_type= value ? *(ulong*) value : 0UL;
    if (cursor_type > (ulong) CURSOR_TYPE_READ_ONLY)
      goto err_not_implemented;
    stmt->flags= cursor_type;
    break;
  }
  case STMT_ATTR_PREFETCH_ROWS:
  {
    ulong prefetch_rows= value ? *(ulong*) value : DEFAULT_PREFETCH_ROWS;
    if (value == 0)
      return TRUE;
    stmt->prefetch_rows= prefetch_rows;
    break;
  }
  default:
    goto err_not_implemented;
  }
  return FALSE;
err_not_implemented:
  set_stmt_error(stmt, CR_NOT_IMPLEMENTED, unknown_sqlstate, NULL);
  return TRUE;
}