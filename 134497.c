static void megasas_write_sense(MegasasCmd *cmd, SCSISense sense)
{
    uint8_t sense_buf[SCSI_SENSE_BUF_SIZE];
    uint8_t sense_len = 18;

    memset(sense_buf, 0, sense_len);
    sense_buf[0] = 0xf0;
    sense_buf[2] = sense.key;
    sense_buf[7] = 10;
    sense_buf[12] = sense.asc;
    sense_buf[13] = sense.ascq;
    megasas_build_sense(cmd, sense_buf, sense_len);
}