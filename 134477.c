static QEMUSGList *megasas_get_sg_list(SCSIRequest *req)
{
    MegasasCmd *cmd = req->hba_private;

    if (cmd->frame->header.frame_cmd == MFI_CMD_DCMD) {
        return NULL;
    } else {
        return &cmd->qsg;
    }
}