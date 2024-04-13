iscsi_allocmap_set_unallocated(IscsiLun *iscsilun, int64_t offset,
                               int64_t bytes)
{
    /* Note: if cache.direct=on the fifth argument to iscsi_allocmap_update
     * is ignored, so this will in effect be an iscsi_allocmap_set_invalid.
     */
    iscsi_allocmap_update(iscsilun, offset, bytes, false, true);
}