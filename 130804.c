sig_handler mysql_end(int sig)
{
#ifndef _WIN32
  /*
    Ingnoring SIGQUIT and SIGINT signals when cleanup process starts.
    This will help in resolving the double free issues, which occures in case
    the signal handler function is started in between the clean up function.
  */
  signal(SIGQUIT, SIG_IGN);
  signal(SIGINT, SIG_IGN);
#endif

  mysql_close(&mysql);
#ifdef HAVE_READLINE
  if (!status.batch && !quick && !opt_html && !opt_xml &&
      histfile && histfile[0])
  {
    /* write-history */
    if (verbose)
      tee_fprintf(stdout, "Writing history-file %s\n",histfile);
    if (!write_history(histfile_tmp))
      my_rename(histfile_tmp, histfile, MYF(MY_WME));
  }
  batch_readline_end(status.line_buff);
  completion_hash_free(&ht);
  free_root(&hash_mem_root,MYF(0));

#endif
  if (sig >= 0)
    put_info(sig ? "Aborted" : "Bye", INFO_RESULT);
  glob_buffer.free();
  old_buffer.free();
  processed_prompt.free();
  my_free(server_version);
  my_free(opt_password);
  my_free(opt_mysql_unix_port);
  my_free(histfile);
  my_free(histfile_tmp);
  my_free(current_db);
  my_free(current_host);
  my_free(current_user);
  my_free(full_username);
  my_free(part_username);
  my_free(default_prompt);
#ifdef HAVE_SMEM
  my_free(shared_memory_base_name);
#endif
  my_free(current_prompt);
  while (embedded_server_arg_count > 1)
    my_free(embedded_server_args[--embedded_server_arg_count]);
  mysql_server_end();
  free_defaults(defaults_argv);
  my_end(my_end_arg);
  exit(status.exit_status);
}