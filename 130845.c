static int com_prompt(String *buffer __attribute__((unused)),
                      char *line)
{
  char *ptr=strchr(line, ' ');
  prompt_counter = 0;
  my_free(current_prompt);
  current_prompt=my_strdup(ptr ? ptr+1 : default_prompt,MYF(MY_WME));
  if (!ptr)
    tee_fprintf(stdout, "Returning to default PROMPT of %s\n", default_prompt);
  else
    tee_fprintf(stdout, "PROMPT set to '%s'\n", current_prompt);
  return 0;
}