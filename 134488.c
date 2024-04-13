static int megasas_finish_internal_dcmd(MegasasCmd *cmd,
                                        SCSIRequest *req)
{
    int opcode;
    int retval = MFI_STAT_OK;
    int lun = req->lun;

    opcode = le32_to_cpu(cmd->frame->dcmd.opcode);
    trace_megasas_dcmd_internal_finish(cmd->index, opcode, lun);
    switch (opcode) {
    case MFI_DCMD_PD_GET_INFO:
        retval = megasas_pd_get_info_submit(req->dev, lun, cmd);
        break;
    case MFI_DCMD_LD_GET_INFO:
        retval = megasas_ld_get_info_submit(req->dev, lun, cmd);
        break;
    default:
        trace_megasas_dcmd_internal_invalid(cmd->index, opcode);
        retval = MFI_STAT_INVALID_DCMD;
        break;
    }
    if (retval != MFI_STAT_INVALID_STATUS) {
        megasas_finish_dcmd(cmd, cmd->iov_size);
    }
    return retval;
}