static uint16_t nvme_io_cmd(NvmeCtrl *n, NvmeRequest *req)
{
    NvmeNamespace *ns;
    uint32_t nsid = le32_to_cpu(req->cmd.nsid);

    trace_pci_nvme_io_cmd(nvme_cid(req), nsid, nvme_sqid(req),
                          req->cmd.opcode, nvme_io_opc_str(req->cmd.opcode));

    if (!nvme_nsid_valid(n, nsid)) {
        return NVME_INVALID_NSID | NVME_DNR;
    }

    /*
     * In the base NVM command set, Flush may apply to all namespaces
     * (indicated by NSID being set to FFFFFFFFh). But if that feature is used
     * along with TP 4056 (Namespace Types), it may be pretty screwed up.
     *
     * If NSID is indeed set to FFFFFFFFh, we simply cannot associate the
     * opcode with a specific command since we cannot determine a unique I/O
     * command set. Opcode 0h could have any other meaning than something
     * equivalent to flushing and say it DOES have completely different
     * semantics in some other command set - does an NSID of FFFFFFFFh then
     * mean "for all namespaces, apply whatever command set specific command
     * that uses the 0h opcode?" Or does it mean "for all namespaces, apply
     * whatever command that uses the 0h opcode if, and only if, it allows NSID
     * to be FFFFFFFFh"?
     *
     * Anyway (and luckily), for now, we do not care about this since the
     * device only supports namespace types that includes the NVM Flush command
     * (NVM and Zoned), so always do an NVM Flush.
     */
    if (req->cmd.opcode == NVME_CMD_FLUSH) {
        return nvme_flush(n, req);
    }

    ns = nvme_ns(n, nsid);
    if (unlikely(!ns)) {
        return NVME_INVALID_FIELD | NVME_DNR;
    }

    if (!(ns->iocs[req->cmd.opcode] & NVME_CMD_EFF_CSUPP)) {
        trace_pci_nvme_err_invalid_opc(req->cmd.opcode);
        return NVME_INVALID_OPCODE | NVME_DNR;
    }

    if (ns->status) {
        return ns->status;
    }

    if (NVME_CMD_FLAGS_FUSE(req->cmd.flags)) {
        return NVME_INVALID_FIELD;
    }

    req->ns = ns;

    switch (req->cmd.opcode) {
    case NVME_CMD_WRITE_ZEROES:
        return nvme_write_zeroes(n, req);
    case NVME_CMD_ZONE_APPEND:
        return nvme_zone_append(n, req);
    case NVME_CMD_WRITE:
        return nvme_write(n, req);
    case NVME_CMD_READ:
        return nvme_read(n, req);
    case NVME_CMD_COMPARE:
        return nvme_compare(n, req);
    case NVME_CMD_DSM:
        return nvme_dsm(n, req);
    case NVME_CMD_VERIFY:
        return nvme_verify(n, req);
    case NVME_CMD_COPY:
        return nvme_copy(n, req);
    case NVME_CMD_ZONE_MGMT_SEND:
        return nvme_zone_mgmt_send(n, req);
    case NVME_CMD_ZONE_MGMT_RECV:
        return nvme_zone_mgmt_recv(n, req);
    default:
        assert(false);
    }

    return NVME_INVALID_OPCODE | NVME_DNR;
}