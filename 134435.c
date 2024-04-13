static void megasas_copy_sense(MegasasCmd *cmd)
{
    uint8_t sense_buf[SCSI_SENSE_BUF_SIZE];
    uint8_t sense_len;

    sense_len = scsi_req_get_sense(cmd->req, sense_buf,
                                   SCSI_SENSE_BUF_SIZE);
    megasas_build_sense(cmd, sense_buf, sense_len);
}