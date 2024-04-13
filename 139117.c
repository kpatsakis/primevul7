my_bool STDCALL mysql_stmt_free_result(MYSQL_STMT *stmt)
{
  DBUG_ENTER("mysql_stmt_free_result");

  /* Free the client side and close the server side cursor if there is one */
  DBUG_RETURN(reset_stmt_handle(stmt, RESET_LONG_DATA | RESET_STORE_RESULT |
                                RESET_CLEAR_ERROR));
}