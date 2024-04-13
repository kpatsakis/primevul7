static void initialize_readline (char *name)
{
  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = name;

  /* Tell the completer that we want a crack first. */
#if defined(USE_NEW_READLINE_INTERFACE)
  rl_attempted_completion_function= (rl_completion_func_t*)&new_mysql_completion;
  rl_completion_entry_function= (rl_compentry_func_t*)&no_completion;

  rl_add_defun("magic-space", (rl_command_func_t *)&fake_magic_space, -1);
#elif defined(USE_LIBEDIT_INTERFACE)
#ifdef HAVE_LOCALE_H
  setlocale(LC_ALL,""); /* so as libedit use isprint */
#endif
  rl_attempted_completion_function= (CPPFunction*)&new_mysql_completion;
  rl_completion_entry_function= &no_completion;
  rl_add_defun("magic-space", (Function*)&fake_magic_space, -1);
#else
  rl_attempted_completion_function= (CPPFunction*)&new_mysql_completion;
  rl_completion_entry_function= &no_completion;
#endif
}