void stat_remove_node(SpiceServer *reds, RedStatNode *node)
{
    if (node->ref != INVALID_STAT_REF) {
        stat_file_remove_node(reds->stat_file, node->ref);
        node->ref = INVALID_STAT_REF;
    }
}