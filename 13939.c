reds_core_timer_add_internal(RedsState *reds,
                             SpiceTimerFunc func,
                             void *opaque)
{
   g_return_val_if_fail(reds != NULL, NULL);
   g_return_val_if_fail(reds->core.timer_add != NULL, NULL);

   return reds->core.timer_add(&reds->core, func, opaque);

}