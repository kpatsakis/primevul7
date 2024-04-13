static void nvme_write_bar(NvmeCtrl *n, hwaddr offset, uint64_t data,
                           unsigned size)
{
    uint64_t cap = ldq_le_p(&n->bar.cap);
    uint32_t cc = ldl_le_p(&n->bar.cc);
    uint32_t intms = ldl_le_p(&n->bar.intms);
    uint32_t csts = ldl_le_p(&n->bar.csts);
    uint32_t pmrsts = ldl_le_p(&n->bar.pmrsts);

    if (unlikely(offset & (sizeof(uint32_t) - 1))) {
        NVME_GUEST_ERR(pci_nvme_ub_mmiowr_misaligned32,
                       "MMIO write not 32-bit aligned,"
                       " offset=0x%"PRIx64"", offset);
        /* should be ignored, fall through for now */
    }

    if (unlikely(size < sizeof(uint32_t))) {
        NVME_GUEST_ERR(pci_nvme_ub_mmiowr_toosmall,
                       "MMIO write smaller than 32-bits,"
                       " offset=0x%"PRIx64", size=%u",
                       offset, size);
        /* should be ignored, fall through for now */
    }

    switch (offset) {
    case NVME_REG_INTMS:
        if (unlikely(msix_enabled(&(n->parent_obj)))) {
            NVME_GUEST_ERR(pci_nvme_ub_mmiowr_intmask_with_msix,
                           "undefined access to interrupt mask set"
                           " when MSI-X is enabled");
            /* should be ignored, fall through for now */
        }
        intms |= data;
        stl_le_p(&n->bar.intms, intms);
        n->bar.intmc = n->bar.intms;
        trace_pci_nvme_mmio_intm_set(data & 0xffffffff, intms);
        nvme_irq_check(n);
        break;
    case NVME_REG_INTMC:
        if (unlikely(msix_enabled(&(n->parent_obj)))) {
            NVME_GUEST_ERR(pci_nvme_ub_mmiowr_intmask_with_msix,
                           "undefined access to interrupt mask clr"
                           " when MSI-X is enabled");
            /* should be ignored, fall through for now */
        }
        intms &= ~data;
        stl_le_p(&n->bar.intms, intms);
        n->bar.intmc = n->bar.intms;
        trace_pci_nvme_mmio_intm_clr(data & 0xffffffff, intms);
        nvme_irq_check(n);
        break;
    case NVME_REG_CC:
        trace_pci_nvme_mmio_cfg(data & 0xffffffff);

        /* Windows first sends data, then sends enable bit */
        if (!NVME_CC_EN(data) && !NVME_CC_EN(cc) &&
            !NVME_CC_SHN(data) && !NVME_CC_SHN(cc))
        {
            cc = data;
        }

        if (NVME_CC_EN(data) && !NVME_CC_EN(cc)) {
            cc = data;

            /* flush CC since nvme_start_ctrl() needs the value */
            stl_le_p(&n->bar.cc, cc);
            if (unlikely(nvme_start_ctrl(n))) {
                trace_pci_nvme_err_startfail();
                csts = NVME_CSTS_FAILED;
            } else {
                trace_pci_nvme_mmio_start_success();
                csts = NVME_CSTS_READY;
            }
        } else if (!NVME_CC_EN(data) && NVME_CC_EN(cc)) {
            trace_pci_nvme_mmio_stopped();
            nvme_ctrl_reset(n);
            cc = 0;
            csts &= ~NVME_CSTS_READY;
        }

        if (NVME_CC_SHN(data) && !(NVME_CC_SHN(cc))) {
            trace_pci_nvme_mmio_shutdown_set();
            nvme_ctrl_shutdown(n);
            cc = data;
            csts |= NVME_CSTS_SHST_COMPLETE;
        } else if (!NVME_CC_SHN(data) && NVME_CC_SHN(cc)) {
            trace_pci_nvme_mmio_shutdown_cleared();
            csts &= ~NVME_CSTS_SHST_COMPLETE;
            cc = data;
        }

        stl_le_p(&n->bar.cc, cc);
        stl_le_p(&n->bar.csts, csts);

        break;
    case NVME_REG_CSTS:
        if (data & (1 << 4)) {
            NVME_GUEST_ERR(pci_nvme_ub_mmiowr_ssreset_w1c_unsupported,
                           "attempted to W1C CSTS.NSSRO"
                           " but CAP.NSSRS is zero (not supported)");
        } else if (data != 0) {
            NVME_GUEST_ERR(pci_nvme_ub_mmiowr_ro_csts,
                           "attempted to set a read only bit"
                           " of controller status");
        }
        break;
    case NVME_REG_NSSR:
        if (data == 0x4e564d65) {
            trace_pci_nvme_ub_mmiowr_ssreset_unsupported();
        } else {
            /* The spec says that writes of other values have no effect */
            return;
        }
        break;
    case NVME_REG_AQA:
        stl_le_p(&n->bar.aqa, data);
        trace_pci_nvme_mmio_aqattr(data & 0xffffffff);
        break;
    case NVME_REG_ASQ:
        stn_le_p(&n->bar.asq, size, data);
        trace_pci_nvme_mmio_asqaddr(data);
        break;
    case NVME_REG_ASQ + 4:
        stl_le_p((uint8_t *)&n->bar.asq + 4, data);
        trace_pci_nvme_mmio_asqaddr_hi(data, ldq_le_p(&n->bar.asq));
        break;
    case NVME_REG_ACQ:
        trace_pci_nvme_mmio_acqaddr(data);
        stn_le_p(&n->bar.acq, size, data);
        break;
    case NVME_REG_ACQ + 4:
        stl_le_p((uint8_t *)&n->bar.acq + 4, data);
        trace_pci_nvme_mmio_acqaddr_hi(data, ldq_le_p(&n->bar.acq));
        break;
    case NVME_REG_CMBLOC:
        NVME_GUEST_ERR(pci_nvme_ub_mmiowr_cmbloc_reserved,
                       "invalid write to reserved CMBLOC"
                       " when CMBSZ is zero, ignored");
        return;
    case NVME_REG_CMBSZ:
        NVME_GUEST_ERR(pci_nvme_ub_mmiowr_cmbsz_readonly,
                       "invalid write to read only CMBSZ, ignored");
        return;
    case NVME_REG_CMBMSC:
        if (!NVME_CAP_CMBS(cap)) {
            return;
        }

        stn_le_p(&n->bar.cmbmsc, size, data);
        n->cmb.cmse = false;

        if (NVME_CMBMSC_CRE(data)) {
            nvme_cmb_enable_regs(n);

            if (NVME_CMBMSC_CMSE(data)) {
                uint64_t cmbmsc = ldq_le_p(&n->bar.cmbmsc);
                hwaddr cba = NVME_CMBMSC_CBA(cmbmsc) << CMBMSC_CBA_SHIFT;
                if (cba + int128_get64(n->cmb.mem.size) < cba) {
                    uint32_t cmbsts = ldl_le_p(&n->bar.cmbsts);
                    NVME_CMBSTS_SET_CBAI(cmbsts, 1);
                    stl_le_p(&n->bar.cmbsts, cmbsts);
                    return;
                }

                n->cmb.cba = cba;
                n->cmb.cmse = true;
            }
        } else {
            n->bar.cmbsz = 0;
            n->bar.cmbloc = 0;
        }

        return;
    case NVME_REG_CMBMSC + 4:
        stl_le_p((uint8_t *)&n->bar.cmbmsc + 4, data);
        return;

    case NVME_REG_PMRCAP:
        NVME_GUEST_ERR(pci_nvme_ub_mmiowr_pmrcap_readonly,
                       "invalid write to PMRCAP register, ignored");
        return;
    case NVME_REG_PMRCTL:
        if (!NVME_CAP_PMRS(cap)) {
            return;
        }

        stl_le_p(&n->bar.pmrctl, data);
        if (NVME_PMRCTL_EN(data)) {
            memory_region_set_enabled(&n->pmr.dev->mr, true);
            pmrsts = 0;
        } else {
            memory_region_set_enabled(&n->pmr.dev->mr, false);
            NVME_PMRSTS_SET_NRDY(pmrsts, 1);
            n->pmr.cmse = false;
        }
        stl_le_p(&n->bar.pmrsts, pmrsts);
        return;
    case NVME_REG_PMRSTS:
        NVME_GUEST_ERR(pci_nvme_ub_mmiowr_pmrsts_readonly,
                       "invalid write to PMRSTS register, ignored");
        return;
    case NVME_REG_PMREBS:
        NVME_GUEST_ERR(pci_nvme_ub_mmiowr_pmrebs_readonly,
                       "invalid write to PMREBS register, ignored");
        return;
    case NVME_REG_PMRSWTP:
        NVME_GUEST_ERR(pci_nvme_ub_mmiowr_pmrswtp_readonly,
                       "invalid write to PMRSWTP register, ignored");
        return;
    case NVME_REG_PMRMSCL:
        if (!NVME_CAP_PMRS(cap)) {
            return;
        }

        stl_le_p(&n->bar.pmrmscl, data);
        n->pmr.cmse = false;

        if (NVME_PMRMSCL_CMSE(data)) {
            uint64_t pmrmscu = ldl_le_p(&n->bar.pmrmscu);
            hwaddr cba = pmrmscu << 32 |
                (NVME_PMRMSCL_CBA(data) << PMRMSCL_CBA_SHIFT);
            if (cba + int128_get64(n->pmr.dev->mr.size) < cba) {
                NVME_PMRSTS_SET_CBAI(pmrsts, 1);
                stl_le_p(&n->bar.pmrsts, pmrsts);
                return;
            }

            n->pmr.cmse = true;
            n->pmr.cba = cba;
        }

        return;
    case NVME_REG_PMRMSCU:
        if (!NVME_CAP_PMRS(cap)) {
            return;
        }

        stl_le_p(&n->bar.pmrmscu, data);
        return;
    default:
        NVME_GUEST_ERR(pci_nvme_ub_mmiowr_invalid,
                       "invalid MMIO write,"
                       " offset=0x%"PRIx64", data=%"PRIx64"",
                       offset, data);
        break;
    }
}