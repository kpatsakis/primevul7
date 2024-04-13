void STDCALL mysql_server_end()
{
  if (!mysql_client_init)
    return;

  mysql_client_plugin_deinit();

#ifdef EMBEDDED_LIBRARY
  end_embedded_server();
#endif
  finish_client_errs();
  vio_end();

  /* If library called my_init(), free memory allocated by it */
  if (!org_my_init_done)
  {
    my_end(0);
  }
  else
  {
    free_charsets();
    mysql_thread_end();
  }

  mysql_client_init= org_my_init_done= 0;
}