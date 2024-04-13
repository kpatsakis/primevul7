static int megasas_dcmd_pd_list_query(MegasasState *s, MegasasCmd *cmd)
{
    uint16_t flags;

    /* mbox0 contains flags */
    flags = le16_to_cpu(cmd->frame->dcmd.mbox[0]);
    trace_megasas_dcmd_pd_list_query(cmd->index, flags);
    if (flags == MR_PD_QUERY_TYPE_ALL ||
        megasas_is_jbod(s)) {
        return megasas_dcmd_pd_get_list(s, cmd);
    }

    return MFI_STAT_OK;
}