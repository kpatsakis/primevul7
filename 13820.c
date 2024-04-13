void stat_init_node(RedStatNode *node, SpiceServer *reds, const RedStatNode *parent,
                    const char *name, int visible)
{
    StatNodeRef parent_ref = parent ? parent->ref : INVALID_STAT_REF;
    node->ref = stat_file_add_node(reds->stat_file, parent_ref, name, visible);
}