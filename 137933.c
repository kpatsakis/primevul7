static void nvme_init_state(NvmeCtrl *n)
{
    /* add one to max_ioqpairs to account for the admin queue pair */
    n->reg_size = pow2ceil(sizeof(NvmeBar) +
                           2 * (n->params.max_ioqpairs + 1) * NVME_DB_SIZE);
    n->sq = g_new0(NvmeSQueue *, n->params.max_ioqpairs + 1);
    n->cq = g_new0(NvmeCQueue *, n->params.max_ioqpairs + 1);
    n->temperature = NVME_TEMPERATURE;
    n->features.temp_thresh_hi = NVME_TEMPERATURE_WARNING;
    n->starttime_ms = qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL);
    n->aer_reqs = g_new0(NvmeRequest *, n->params.aerl + 1);
}