static void nvme_zrm_auto_transition_zone(NvmeNamespace *ns)
{
    NvmeZone *zone;

    if (ns->params.max_open_zones &&
        ns->nr_open_zones == ns->params.max_open_zones) {
        zone = QTAILQ_FIRST(&ns->imp_open_zones);
        if (zone) {
            /*
             * Automatically close this implicitly open zone.
             */
            QTAILQ_REMOVE(&ns->imp_open_zones, zone, entry);
            nvme_zrm_close(ns, zone);
        }
    }
}