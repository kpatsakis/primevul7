static int process_command(MYSQL_THD thd, LEX_CSTRING event_command,
                           my_bool consume_event)
{
  LEX_CSTRING abort_ret_command= { C_STRING_WITH_LEN("ABORT_RET") };

  if (!my_charset_latin1.coll->strnncoll(&my_charset_latin1,
                                         (const uchar *)event_command.str,
                                         event_command.length,
                                         (const uchar *)abort_ret_command.str,
                                         abort_ret_command.length, 0))
  {
    int ret_code = (int)THDVAR(thd, abort_value);
    const char *err_message = (const char *)THDVAR(thd, abort_message);
    LEX_CSTRING status = { C_STRING_WITH_LEN("EVENT-ORDER-ABORT") };
    LEX_CSTRING order_cstr;

    lex_cstring_set(&order_cstr, 
                    (const char *)THDVAR(thd, event_order_check));

    /* Do not replace order string yet. */
    if (consume_event)
    {
      memmove((char *) order_cstr.str,
              (void *) status.str, status.length + 1);

      THDVAR(thd, abort_value)= 1;
      THDVAR(thd, abort_message)= 0;
    }

    if (err_message)
    {
      my_message(ER_AUDIT_API_ABORT, err_message, MYF(0));
      THDVAR(thd, event_order_check)= (char *)order_cstr.str;
    }

    return ret_code;
  }

  return 0;
}