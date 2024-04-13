SpiceWatch *reds_core_watch_add(RedsState *reds,
                                int fd, int event_mask,
                                SpiceWatchFunc func,
                                void *opaque)
{
   g_return_val_if_fail(reds != NULL, NULL);
   g_return_val_if_fail(reds->core.watch_add != NULL, NULL);

   return reds->core.watch_add(&reds->core, fd, event_mask, func, opaque);
}