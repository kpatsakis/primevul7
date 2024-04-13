static void megasas_soft_reset(MegasasState *s)
{
    int i;
    MegasasCmd *cmd;

    trace_megasas_reset(s->fw_state);
    for (i = 0; i < s->fw_cmds; i++) {
        cmd = &s->frames[i];
        megasas_abort_command(cmd);
    }
    if (s->fw_state == MFI_FWSTATE_READY) {
        BusChild *kid;

        /*
         * The EFI firmware doesn't handle UA,
         * so we need to clear the Power On/Reset UA
         * after the initial reset.
         */
        QTAILQ_FOREACH(kid, &s->bus.qbus.children, sibling) {
            SCSIDevice *sdev = SCSI_DEVICE(kid->child);

            sdev->unit_attention = SENSE_CODE(NO_SENSE);
            scsi_device_unit_attention_reported(sdev);
        }
    }
    megasas_reset_frames(s);
    s->reply_queue_len = s->fw_cmds;
    s->reply_queue_pa = 0;
    s->consumer_pa = 0;
    s->producer_pa = 0;
    s->fw_state = MFI_FWSTATE_READY;
    s->doorbell = 0;
    s->intr_mask = MEGASAS_INTR_DISABLED_MASK;
    s->frame_hi = 0;
    s->flags &= ~MEGASAS_MASK_USE_QUEUE64;
    s->event_count++;
    s->boot_event = s->event_count;
}