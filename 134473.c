static int megasas_handle_scsi(MegasasState *s, MegasasCmd *cmd,
                               bool is_logical)
{
    uint8_t *cdb;
    bool is_write;
    struct SCSIDevice *sdev = NULL;

    cdb = cmd->frame->pass.cdb;

    if (is_logical) {
        if (cmd->frame->header.target_id >= MFI_MAX_LD ||
            cmd->frame->header.lun_id != 0) {
            trace_megasas_scsi_target_not_present(
                mfi_frame_desc[cmd->frame->header.frame_cmd], is_logical,
                cmd->frame->header.target_id, cmd->frame->header.lun_id);
            return MFI_STAT_DEVICE_NOT_FOUND;
        }
    }
    sdev = scsi_device_find(&s->bus, 0, cmd->frame->header.target_id,
                            cmd->frame->header.lun_id);

    cmd->iov_size = le32_to_cpu(cmd->frame->header.data_len);
    trace_megasas_handle_scsi(mfi_frame_desc[cmd->frame->header.frame_cmd],
                              is_logical, cmd->frame->header.target_id,
                              cmd->frame->header.lun_id, sdev, cmd->iov_size);

    if (!sdev || (megasas_is_jbod(s) && is_logical)) {
        trace_megasas_scsi_target_not_present(
            mfi_frame_desc[cmd->frame->header.frame_cmd], is_logical,
            cmd->frame->header.target_id, cmd->frame->header.lun_id);
        return MFI_STAT_DEVICE_NOT_FOUND;
    }

    if (cmd->frame->header.cdb_len > 16) {
        trace_megasas_scsi_invalid_cdb_len(
                mfi_frame_desc[cmd->frame->header.frame_cmd], is_logical,
                cmd->frame->header.target_id, cmd->frame->header.lun_id,
                cmd->frame->header.cdb_len);
        megasas_write_sense(cmd, SENSE_CODE(INVALID_OPCODE));
        cmd->frame->header.scsi_status = CHECK_CONDITION;
        s->event_count++;
        return MFI_STAT_SCSI_DONE_WITH_ERROR;
    }

    if (megasas_map_sgl(s, cmd, &cmd->frame->pass.sgl)) {
        megasas_write_sense(cmd, SENSE_CODE(TARGET_FAILURE));
        cmd->frame->header.scsi_status = CHECK_CONDITION;
        s->event_count++;
        return MFI_STAT_SCSI_DONE_WITH_ERROR;
    }

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

    is_write = (cmd->req->cmd.mode == SCSI_XFER_TO_DEV);
    if (cmd->iov_size) {
        if (is_write) {
            trace_megasas_scsi_write_start(cmd->index, cmd->iov_size);
        } else {
            trace_megasas_scsi_read_start(cmd->index, cmd->iov_size);
        }
    } else {
        trace_megasas_scsi_nodata(cmd->index);
    }
    megasas_enqueue_req(cmd, is_write);
    return MFI_STAT_INVALID_STATUS;
}