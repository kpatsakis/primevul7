static void megasas_complete_command(MegasasCmd *cmd)
{
    qemu_sglist_destroy(&cmd->qsg);
    cmd->iov_size = 0;
    cmd->iov_offset = 0;

    cmd->req->hba_private = NULL;
    scsi_req_unref(cmd->req);
    cmd->req = NULL;

    megasas_unmap_frame(cmd->state, cmd);
    megasas_complete_frame(cmd->state, cmd->context);
}