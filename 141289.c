brcmf_cfg80211_escan_handler(struct brcmf_if *ifp,
			     const struct brcmf_event_msg *e, void *data)
{
	struct brcmf_cfg80211_info *cfg = ifp->drvr->config;
	struct wiphy *wiphy = cfg_to_wiphy(cfg);
	s32 status;
	struct brcmf_escan_result_le *escan_result_le;
	u32 escan_buflen;
	struct brcmf_bss_info_le *bss_info_le;
	struct brcmf_bss_info_le *bss = NULL;
	u32 bi_length;
	struct brcmf_scan_results *list;
	u32 i;
	bool aborted;

	status = e->status;

	if (status == BRCMF_E_STATUS_ABORT)
		goto exit;

	if (!test_bit(BRCMF_SCAN_STATUS_BUSY, &cfg->scan_status)) {
		bphy_err(wiphy, "scan not ready, bsscfgidx=%d\n",
			 ifp->bsscfgidx);
		return -EPERM;
	}

	if (status == BRCMF_E_STATUS_PARTIAL) {
		brcmf_dbg(SCAN, "ESCAN Partial result\n");
		if (e->datalen < sizeof(*escan_result_le)) {
			bphy_err(wiphy, "invalid event data length\n");
			goto exit;
		}
		escan_result_le = (struct brcmf_escan_result_le *) data;
		if (!escan_result_le) {
			bphy_err(wiphy, "Invalid escan result (NULL pointer)\n");
			goto exit;
		}
		escan_buflen = le32_to_cpu(escan_result_le->buflen);
		if (escan_buflen > BRCMF_ESCAN_BUF_SIZE ||
		    escan_buflen > e->datalen ||
		    escan_buflen < sizeof(*escan_result_le)) {
			bphy_err(wiphy, "Invalid escan buffer length: %d\n",
				 escan_buflen);
			goto exit;
		}
		if (le16_to_cpu(escan_result_le->bss_count) != 1) {
			bphy_err(wiphy, "Invalid bss_count %d: ignoring\n",
				 escan_result_le->bss_count);
			goto exit;
		}
		bss_info_le = &escan_result_le->bss_info_le;

		if (brcmf_p2p_scan_finding_common_channel(cfg, bss_info_le))
			goto exit;

		if (!cfg->int_escan_map && !cfg->scan_request) {
			brcmf_dbg(SCAN, "result without cfg80211 request\n");
			goto exit;
		}

		bi_length = le32_to_cpu(bss_info_le->length);
		if (bi_length != escan_buflen -	WL_ESCAN_RESULTS_FIXED_SIZE) {
			bphy_err(wiphy, "Ignoring invalid bss_info length: %d\n",
				 bi_length);
			goto exit;
		}

		if (!(cfg_to_wiphy(cfg)->interface_modes &
					BIT(NL80211_IFTYPE_ADHOC))) {
			if (le16_to_cpu(bss_info_le->capability) &
						WLAN_CAPABILITY_IBSS) {
				bphy_err(wiphy, "Ignoring IBSS result\n");
				goto exit;
			}
		}

		list = (struct brcmf_scan_results *)
				cfg->escan_info.escan_buf;
		if (bi_length > BRCMF_ESCAN_BUF_SIZE - list->buflen) {
			bphy_err(wiphy, "Buffer is too small: ignoring\n");
			goto exit;
		}

		for (i = 0; i < list->count; i++) {
			bss = bss ? (struct brcmf_bss_info_le *)
				((unsigned char *)bss +
				le32_to_cpu(bss->length)) : list->bss_info_le;
			if (brcmf_compare_update_same_bss(cfg, bss,
							  bss_info_le))
				goto exit;
		}
		memcpy(&cfg->escan_info.escan_buf[list->buflen], bss_info_le,
		       bi_length);
		list->version = le32_to_cpu(bss_info_le->version);
		list->buflen += bi_length;
		list->count++;
	} else {
		cfg->escan_info.escan_state = WL_ESCAN_STATE_IDLE;
		if (brcmf_p2p_scan_finding_common_channel(cfg, NULL))
			goto exit;
		if (cfg->int_escan_map || cfg->scan_request) {
			brcmf_inform_bss(cfg);
			aborted = status != BRCMF_E_STATUS_SUCCESS;
			brcmf_notify_escan_complete(cfg, ifp, aborted, false);
		} else
			brcmf_dbg(SCAN, "Ignored scan complete result 0x%x\n",
				  status);
	}
exit:
	return 0;
}