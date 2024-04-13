void reds_on_vm_stop(RedsState *reds)
{
    FOREACH_QXL_INSTANCE(reds, qxl) {
        red_qxl_stop(qxl);
    }
}