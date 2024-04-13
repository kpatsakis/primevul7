static int audit_null_plugin_init(void *arg MY_ATTRIBUTE((unused)))
{
  struct st_mysql_show_var *var;

  for (var= simple_status; var->value != 0; var++)
  {
    *((int*)var->value) = 0;
  }

  return(0);
}