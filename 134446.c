static MegasasCmd *megasas_enqueue_frame(MegasasState *s,
    hwaddr frame, uint64_t context, int count)
{
    PCIDevice *pcid = PCI_DEVICE(s);
    MegasasCmd *cmd = NULL;
    int frame_size = MFI_FRAME_SIZE * 16;
    hwaddr frame_size_p = frame_size;
    unsigned long index;

    index = 0;
    while (index < s->fw_cmds) {
        index = find_next_zero_bit(s->frame_map, s->fw_cmds, index);
        if (!s->frames[index].pa)
            break;
        /* Busy frame found */
        trace_megasas_qf_mapped(index);
    }
    if (index >= s->fw_cmds) {
        /* All frames busy */
        trace_megasas_qf_busy(frame);
        return NULL;
    }
    cmd = &s->frames[index];
    set_bit(index, s->frame_map);
    trace_megasas_qf_new(index, frame);

    cmd->pa = frame;
    /* Map all possible frames */
    cmd->frame = pci_dma_map(pcid, frame, &frame_size_p, 0);
    if (frame_size_p != frame_size) {
        trace_megasas_qf_map_failed(cmd->index, (unsigned long)frame);
        if (cmd->frame) {
            megasas_unmap_frame(s, cmd);
        }
        s->event_count++;
        return NULL;
    }
    cmd->pa_size = frame_size_p;
    cmd->context = context;
    if (!megasas_use_queue64(s)) {
        cmd->context &= (uint64_t)0xFFFFFFFF;
    }
    cmd->count = count;
    s->busy++;

    if (s->consumer_pa) {
        s->reply_queue_tail = ldl_le_pci_dma(pcid, s->consumer_pa);
    }
    trace_megasas_qf_enqueue(cmd->index, cmd->count, cmd->context,
                             s->reply_queue_head, s->reply_queue_tail, s->busy);

    return cmd;
}