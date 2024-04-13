static void megasas_xfer_complete(SCSIRequest *req, uint32_t len)
{
    MegasasCmd *cmd = req->hba_private;
    uint8_t *buf;
    uint32_t opcode;

    trace_megasas_io_complete(cmd->index, len);

    if (cmd->frame->header.frame_cmd != MFI_CMD_DCMD) {
        scsi_req_continue(req);
        return;
    }

    buf = scsi_req_get_buf(req);
    opcode = le32_to_cpu(cmd->frame->dcmd.opcode);
    if (opcode == MFI_DCMD_PD_GET_INFO && cmd->iov_buf) {
        struct mfi_pd_info *info = cmd->iov_buf;

        if (info->inquiry_data[0] == 0x7f) {
            memset(info->inquiry_data, 0, sizeof(info->inquiry_data));
            memcpy(info->inquiry_data, buf, len);
        } else if (info->vpd_page83[0] == 0x7f) {
            memset(info->vpd_page83, 0, sizeof(info->vpd_page83));
            memcpy(info->vpd_page83, buf, len);
        }
        scsi_req_continue(req);
    } else if (opcode == MFI_DCMD_LD_GET_INFO) {
        struct mfi_ld_info *info = cmd->iov_buf;

        if (cmd->iov_buf) {
            memcpy(info->vpd_page83, buf, sizeof(info->vpd_page83));
            scsi_req_continue(req);
        }
    }
}