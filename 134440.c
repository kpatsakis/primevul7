static int megasas_setup_inquiry(uint8_t *cdb, int pg, int len)
{
    memset(cdb, 0, 6);
    cdb[0] = INQUIRY;
    if (pg > 0) {
        cdb[1] = 0x1;
        cdb[2] = pg;
    }
    cdb[3] = (len >> 8) & 0xff;
    cdb[4] = (len & 0xff);
    return len;
}