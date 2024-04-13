void reds_on_vm_start(RedsState *reds)
{
    FOREACH_QXL_INSTANCE(reds, qxl) {
        red_qxl_start(qxl);
    }
}