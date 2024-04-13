static int megasas_handle_io(MegasasState *s, MegasasCmd *cmd)
{
    uint32_t lba_count, lba_start_hi, lba_start_lo;
    uint64_t lba_start;
    bool is_write = (cmd->frame->header.frame_cmd == MFI_CMD_LD_WRITE);
    uint8_t cdb[16];
    int len;
    struct SCSIDevice *sdev = NULL;

    lba_count = le32_to_cpu(cmd->frame->io.header.data_len);
    lba_start_lo = le32_to_cpu(cmd->frame->io.lba_lo);
    lba_start_hi = le32_to_cpu(cmd->frame->io.lba_hi);
    lba_start = ((uint64_t)lba_start_hi << 32) | lba_start_lo;

    if (cmd->frame->header.target_id < MFI_MAX_LD &&
        cmd->frame->header.lun_id == 0) {
        sdev = scsi_device_find(&s->bus, 0, cmd->frame->header.target_id,
                                cmd->frame->header.lun_id);
    }

    trace_megasas_handle_io(cmd->index,
                            mfi_frame_desc[cmd->frame->header.frame_cmd],
                            cmd->frame->header.target_id,
                            cmd->frame->header.lun_id,
                            (unsigned long)lba_start, (unsigned long)lba_count);
    if (!sdev) {
        trace_megasas_io_target_not_present(cmd->index,
            mfi_frame_desc[cmd->frame->header.frame_cmd],
            cmd->frame->header.target_id, cmd->frame->header.lun_id);
        return MFI_STAT_DEVICE_NOT_FOUND;
    }

    if (cmd->frame->header.cdb_len > 16) {
        trace_megasas_scsi_invalid_cdb_len(
            mfi_frame_desc[cmd->frame->header.frame_cmd], 1,
            cmd->frame->header.target_id, cmd->frame->header.lun_id,
            cmd->frame->header.cdb_len);
        megasas_write_sense(cmd, SENSE_CODE(INVALID_OPCODE));
        cmd->frame->header.scsi_status = CHECK_CONDITION;
        s->event_count++;
        return MFI_STAT_SCSI_DONE_WITH_ERROR;
    }

    cmd->iov_size = lba_count * sdev->blocksize;
    if (megasas_map_sgl(s, cmd, &cmd->frame->io.sgl)) {
        megasas_write_sense(cmd, SENSE_CODE(TARGET_FAILURE));
        cmd->frame->header.scsi_status = CHECK_CONDITION;
        s->event_count++;
        return MFI_STAT_SCSI_DONE_WITH_ERROR;
    }

    megasas_encode_lba(cdb, lba_start, lba_count, is_write);
    cmd->req = scsi_req_new(sdev, cmd->index,
                            cmd->frame->header.lun_id, cdb, cmd);
    if (!cmd->req) {
        trace_megasas_scsi_req_alloc_failed(
            mfi_frame_desc[cmd->frame->header.frame_cmd],
            cmd->frame->header.target_id, cmd->frame->header.lun_id);
        megasas_write_sense(cmd, SENSE_CODE(NO_SENSE));
        cmd->frame->header.scsi_status = BUSY;
        s->event_count++;
        return MFI_STAT_SCSI_DONE_WITH_ERROR;
    }
    len = megasas_enqueue_req(cmd, is_write);
    if (len > 0) {
        if (is_write) {
            trace_megasas_io_write_start(cmd->index, lba_start, lba_count, len);
        } else {
            trace_megasas_io_read_start(cmd->index, lba_start, lba_count, len);
        }
    }
    return MFI_STAT_INVALID_STATUS;
}