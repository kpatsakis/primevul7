static int megasas_cluster_reset_ld(MegasasState *s, MegasasCmd *cmd)
{
    uint16_t target_id;
    int i;

    /* mbox0 contains the device index */
    target_id = le16_to_cpu(cmd->frame->dcmd.mbox[0]);
    trace_megasas_dcmd_reset_ld(cmd->index, target_id);
    for (i = 0; i < s->fw_cmds; i++) {
        MegasasCmd *tmp_cmd = &s->frames[i];
        if (tmp_cmd->req && tmp_cmd->req->dev->id == target_id) {
            SCSIDevice *d = tmp_cmd->req->dev;
            qdev_reset_all(&d->qdev);
        }
    }
    return MFI_STAT_OK;
}