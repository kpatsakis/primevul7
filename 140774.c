iscsi_allocmap_is_allocated(IscsiLun *iscsilun, int64_t offset,
                            int64_t bytes)
{
    unsigned long size;
    if (iscsilun->allocmap == NULL) {
        return true;
    }
    assert(iscsilun->cluster_size);
    size = DIV_ROUND_UP(offset + bytes, iscsilun->cluster_size);
    return !(find_next_bit(iscsilun->allocmap, size,
                           offset / iscsilun->cluster_size) == size);
}