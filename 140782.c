iscsi_allocmap_update(IscsiLun *iscsilun, int64_t offset,
                      int64_t bytes, bool allocated, bool valid)
{
    int64_t cl_num_expanded, nb_cls_expanded, cl_num_shrunk, nb_cls_shrunk;

    if (iscsilun->allocmap == NULL) {
        return;
    }
    /* expand to entirely contain all affected clusters */
    assert(iscsilun->cluster_size);
    cl_num_expanded = offset / iscsilun->cluster_size;
    nb_cls_expanded = DIV_ROUND_UP(offset + bytes,
                                   iscsilun->cluster_size) - cl_num_expanded;
    /* shrink to touch only completely contained clusters */
    cl_num_shrunk = DIV_ROUND_UP(offset, iscsilun->cluster_size);
    nb_cls_shrunk = (offset + bytes) / iscsilun->cluster_size - cl_num_shrunk;
    if (allocated) {
        bitmap_set(iscsilun->allocmap, cl_num_expanded, nb_cls_expanded);
    } else {
        if (nb_cls_shrunk > 0) {
            bitmap_clear(iscsilun->allocmap, cl_num_shrunk, nb_cls_shrunk);
        }
    }

    if (iscsilun->allocmap_valid == NULL) {
        return;
    }
    if (valid) {
        if (nb_cls_shrunk > 0) {
            bitmap_set(iscsilun->allocmap_valid, cl_num_shrunk, nb_cls_shrunk);
        }
    } else {
        bitmap_clear(iscsilun->allocmap_valid, cl_num_expanded,
                     nb_cls_expanded);
    }
}