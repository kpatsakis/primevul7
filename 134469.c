static int megasas_finish_internal_command(MegasasCmd *cmd,
                                           SCSIRequest *req, size_t resid)
{
    int retval = MFI_STAT_INVALID_CMD;

    if (cmd->frame->header.frame_cmd == MFI_CMD_DCMD) {
        cmd->iov_size -= resid;
        retval = megasas_finish_internal_dcmd(cmd, req);
    }
    return retval;
}