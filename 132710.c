static void add_event(MYSQL_THD thd, const char *var, LEX_CSTRING event,
                      const char *data, size_t data_length)
{
  LEX_CSTRING str;
  size_t size;
  char *buffer;

  lex_cstring_set(&str, var);

  size= str.length + event.length + data_length + 4;
  buffer= (char *)my_malloc(PSI_NOT_INSTRUMENTED, size, MYF(MY_FAE));

  my_snprintf(buffer, size, "%s%s%s;%s;", var, str.length == 0 ? "" : "\n",
              event.str, data);

  buffer[size - (str.length == 0 ? 2 : 1)] = '\0';

  THDVAR(thd, event_record)= buffer;
}