static void reds_cleanup(RedsState *reds)
{
#ifdef RED_STATISTICS
    stat_file_unlink(reds->stat_file);
#endif
}