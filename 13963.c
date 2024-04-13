uint32_t reds_get_mm_time(void)
{
    return spice_get_monotonic_time_ns() / NSEC_PER_MILLISEC;
}