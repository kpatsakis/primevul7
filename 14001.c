void stat_init_counter(RedStatCounter *counter, SpiceServer *reds,
                       const RedStatNode *parent, const char *name, int visible)
{
    StatNodeRef parent_ref = parent ? parent->ref : INVALID_STAT_REF;
    counter->counter =
        stat_file_add_counter(reds->stat_file, parent_ref, name, visible);
}