static void megasas_command_cancelled(SCSIRequest *req)
{
    MegasasCmd *cmd = req->hba_private;

    if (!cmd) {
        return;
    }
    cmd->frame->header.cmd_status = MFI_STAT_SCSI_IO_FAILED;
    megasas_complete_command(cmd);
}