static int megasas_event_wait(MegasasState *s, MegasasCmd *cmd)
{
    union mfi_evt event;

    if (cmd->iov_size < sizeof(struct mfi_evt_detail)) {
        trace_megasas_dcmd_invalid_xfer_len(cmd->index, cmd->iov_size,
                                            sizeof(struct mfi_evt_detail));
        return MFI_STAT_INVALID_PARAMETER;
    }
    s->event_count = cpu_to_le32(cmd->frame->dcmd.mbox[0]);
    event.word = cpu_to_le32(cmd->frame->dcmd.mbox[4]);
    s->event_locale = event.members.locale;
    s->event_class = event.members.class;
    s->event_cmd = cmd;
    /* Decrease busy count; event frame doesn't count here */
    s->busy--;
    cmd->iov_size = sizeof(struct mfi_evt_detail);
    return MFI_STAT_INVALID_STATUS;
}