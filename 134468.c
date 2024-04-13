static int megasas_enqueue_req(MegasasCmd *cmd, bool is_write)
{
    int len;

    len = scsi_req_enqueue(cmd->req);
    if (len < 0) {
        len = -len;
    }
    if (len > 0) {
        if (len > cmd->iov_size) {
            if (is_write) {
                trace_megasas_iov_write_overflow(cmd->index, len,
                                                 cmd->iov_size);
            } else {
                trace_megasas_iov_read_overflow(cmd->index, len,
                                                cmd->iov_size);
            }
        }
        if (len < cmd->iov_size) {
            if (is_write) {
                trace_megasas_iov_write_underflow(cmd->index, len,
                                                  cmd->iov_size);
            } else {
                trace_megasas_iov_read_underflow(cmd->index, len,
                                                 cmd->iov_size);
            }
            cmd->iov_size = len;
        }
        scsi_req_continue(cmd->req);
    }
    return len;
}