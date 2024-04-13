my_pipe_sig_handler(int sig __attribute__((unused)))
{
  DBUG_PRINT("info",("Hit by signal %d",sig));
#ifdef SIGNAL_HANDLER_RESET_ON_DELIVERY
  (void) signal(SIGPIPE, my_pipe_sig_handler);
#endif
}