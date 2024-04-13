onig_end(void)
{
  exec_end_call_list();

#ifdef ONIG_DEBUG_STATISTICS
  onig_print_statistics(stderr);
#endif

#if defined(ONIG_DEBUG_MEMLEAK) && defined(_MSC_VER)
  _CrtDumpMemoryLeaks();
#endif

  onig_inited = 0;

  return 0;
}