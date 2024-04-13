void reds_enable_mm_time(RedsState *reds)
{
    reds->mm_time_enabled = TRUE;
    reds->mm_time_latency = MM_TIME_DELTA;
    reds_send_mm_time(reds);
}