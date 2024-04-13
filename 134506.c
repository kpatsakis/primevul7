static void megasas_encode_lba(uint8_t *cdb, uint64_t lba,
                               uint32_t len, bool is_write)
{
    memset(cdb, 0x0, 16);
    if (is_write) {
        cdb[0] = WRITE_16;
    } else {
        cdb[0] = READ_16;
    }
    cdb[2] = (lba >> 56) & 0xff;
    cdb[3] = (lba >> 48) & 0xff;
    cdb[4] = (lba >> 40) & 0xff;
    cdb[5] = (lba >> 32) & 0xff;
    cdb[6] = (lba >> 24) & 0xff;
    cdb[7] = (lba >> 16) & 0xff;
    cdb[8] = (lba >> 8) & 0xff;
    cdb[9] = (lba) & 0xff;
    cdb[10] = (len >> 24) & 0xff;
    cdb[11] = (len >> 16) & 0xff;
    cdb[12] = (len >> 8) & 0xff;
    cdb[13] = (len) & 0xff;
}