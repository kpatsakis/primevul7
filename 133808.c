mirror_wait_for_free_in_flight_slot(MirrorBlockJob *s)
{
    /* Only non-active operations use up in-flight slots */
    mirror_wait_for_any_operation(s, false);
}