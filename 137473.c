static bool cmd_read_dma(IDEState *s, uint8_t cmd)
{
    bool lba48 = (cmd == WIN_READDMA_EXT);

    if (!s->bs) {
        ide_abort_command(s);
        return true;
    }

    ide_cmd_lba48_transform(s, lba48);
    ide_sector_start_dma(s, IDE_DMA_READ);

    return false;
}