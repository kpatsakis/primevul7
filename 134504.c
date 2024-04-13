static int megasas_cache_flush(MegasasState *s, MegasasCmd *cmd)
{
    blk_drain_all();
    return MFI_STAT_OK;
}