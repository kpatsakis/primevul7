static void megasas_abort_command(MegasasCmd *cmd)
{
    /* Never abort internal commands.  */
    if (cmd->req != NULL) {
        scsi_req_cancel(cmd->req);
    }
}