int imap_has_flag (LIST* flag_list, const char* flag)
{
  if (!flag_list)
    return 0;

  flag_list = flag_list->next;
  while (flag_list)
  {
    if (!ascii_strncasecmp (flag_list->data, flag, strlen (flag_list->data)))
      return 1;

    if (!ascii_strncmp (flag_list->data, "\\*", strlen (flag_list->data)))
      return 1;

    flag_list = flag_list->next;
  }

  return 0;
}