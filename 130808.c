static char **new_mysql_completion(const char *text,
                                   int start __attribute__((unused)),
                                   int end __attribute__((unused)))
{
  if (!status.batch && !quick)
#if defined(USE_NEW_READLINE_INTERFACE)
    return rl_completion_matches(text, new_command_generator);
#else
    return completion_matches((char *)text, (CPFunction *)new_command_generator);
#endif
  else
    return (char**) 0;
}