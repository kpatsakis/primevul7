usdhc_write(void *opaque, hwaddr offset, uint64_t val, unsigned size)
{
    SDHCIState *s = SYSBUS_SDHCI(opaque);
    uint8_t hostctl1;
    uint32_t value = (uint32_t)val;

    switch (offset) {
    case ESDHC_DLL_CTRL:
    case ESDHC_TUNE_CTRL_STATUS:
    case ESDHC_UNDOCUMENTED_REG27:
    case ESDHC_TUNING_CTRL:
    case ESDHC_WTMK_LVL:
        break;

    case ESDHC_VENDOR_SPEC:
        s->vendor_spec = value;
        switch (s->vendor) {
        case SDHCI_VENDOR_IMX:
            if (value & ESDHC_IMX_FRC_SDCLK_ON) {
                s->prnsts &= ~SDHC_IMX_CLOCK_GATE_OFF;
            } else {
                s->prnsts |= SDHC_IMX_CLOCK_GATE_OFF;
            }
            break;
        default:
            break;
        }
        break;

    case SDHC_HOSTCTL:
        /*
         * Here's What ESDHCI has at offset 0x28 (SDHC_HOSTCTL)
         *
         *       7         6     5      4      3      2        1      0
         * |-----------+--------+--------+-----------+----------+---------|
         * | Card      | Card   | Endian | DATA3     | Data     | Led     |
         * | Detect    | Detect | Mode   | as Card   | Transfer | Control |
         * | Signal    | Test   |        | Detection | Width    |         |
         * | Selection | Level  |        | Pin       |          |         |
         * |-----------+--------+--------+-----------+----------+---------|
         *
         * and 0x29
         *
         *  15      10 9    8
         * |----------+------|
         * | Reserved | DMA  |
         * |          | Sel. |
         * |          |      |
         * |----------+------|
         *
         * and here's what SDCHI spec expects those offsets to be:
         *
         * 0x28 (Host Control Register)
         *
         *     7        6         5       4  3      2         1        0
         * |--------+--------+----------+------+--------+----------+---------|
         * | Card   | Card   | Extended | DMA  | High   | Data     | LED     |
         * | Detect | Detect | Data     | Sel. | Speed  | Transfer | Control |
         * | Signal | Test   | Transfer |      | Enable | Width    |         |
         * | Sel.   | Level  | Width    |      |        |          |         |
         * |--------+--------+----------+------+--------+----------+---------|
         *
         * and 0x29 (Power Control Register)
         *
         * |----------------------------------|
         * | Power Control Register           |
         * |                                  |
         * | Description omitted,             |
         * | since it has no analog in ESDHCI |
         * |                                  |
         * |----------------------------------|
         *
         * Since offsets 0x2A and 0x2B should be compatible between
         * both IP specs we only need to reconcile least 16-bit of the
         * word we've been given.
         */

        /*
         * First, save bits 7 6 and 0 since they are identical
         */
        hostctl1 = value & (SDHC_CTRL_LED |
                            SDHC_CTRL_CDTEST_INS |
                            SDHC_CTRL_CDTEST_EN);
        /*
         * Second, split "Data Transfer Width" from bits 2 and 1 in to
         * bits 5 and 1
         */
        if (value & ESDHC_CTRL_8BITBUS) {
            hostctl1 |= SDHC_CTRL_8BITBUS;
        }

        if (value & ESDHC_CTRL_4BITBUS) {
            hostctl1 |= ESDHC_CTRL_4BITBUS;
        }

        /*
         * Third, move DMA select from bits 9 and 8 to bits 4 and 3
         */
        hostctl1 |= SDHC_DMA_TYPE(value >> (8 - 3));

        /*
         * Now place the corrected value into low 16-bit of the value
         * we are going to give standard SDHCI write function
         *
         * NOTE: This transformation should be the inverse of what can
         * be found in drivers/mmc/host/sdhci-esdhc-imx.c in Linux
         * kernel
         */
        value &= ~UINT16_MAX;
        value |= hostctl1;
        value |= (uint16_t)s->pwrcon << 8;

        sdhci_write(opaque, offset, value, size);
        break;

    case ESDHC_MIX_CTRL:
        /*
         * So, when SD/MMC stack in Linux tries to write to "Transfer
         * Mode Register", ESDHC i.MX quirk code will translate it
         * into a write to ESDHC_MIX_CTRL, so we do the opposite in
         * order to get where we started
         *
         * Note that Auto CMD23 Enable bit is located in a wrong place
         * on i.MX, but since it is not used by QEMU we do not care.
         *
         * We don't want to call sdhci_write(.., SDHC_TRNMOD, ...)
         * here becuase it will result in a call to
         * sdhci_send_command(s) which we don't want.
         *
         */
        s->trnmod = value & UINT16_MAX;
        break;
    case SDHC_TRNMOD:
        /*
         * Similar to above, but this time a write to "Command
         * Register" will be translated into a 4-byte write to
         * "Transfer Mode register" where lower 16-bit of value would
         * be set to zero. So what we do is fill those bits with
         * cached value from s->trnmod and let the SDHCI
         * infrastructure handle the rest
         */
        sdhci_write(opaque, offset, val | s->trnmod, size);
        break;
    case SDHC_BLKSIZE:
        /*
         * ESDHCI does not implement "Host SDMA Buffer Boundary", and
         * Linux driver will try to zero this field out which will
         * break the rest of SDHCI emulation.
         *
         * Linux defaults to maximum possible setting (512K boundary)
         * and it seems to be the only option that i.MX IP implements,
         * so we artificially set it to that value.
         */
        val |= 0x7 << 12;
        /* FALLTHROUGH */
    default:
        sdhci_write(opaque, offset, val, size);
        break;
    }
}