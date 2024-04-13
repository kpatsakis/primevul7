brcmf_cfg80211_scan(struct wiphy *wiphy, struct cfg80211_scan_request *request)
{
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(wiphy);
	struct brcmf_cfg80211_vif *vif;
	s32 err = 0;

	brcmf_dbg(TRACE, "Enter\n");
	vif = container_of(request->wdev, struct brcmf_cfg80211_vif, wdev);
	if (!check_vif_up(vif))
		return -EIO;

	if (test_bit(BRCMF_SCAN_STATUS_BUSY, &cfg->scan_status)) {
		bphy_err(wiphy, "Scanning already: status (%lu)\n",
			 cfg->scan_status);
		return -EAGAIN;
	}
	if (test_bit(BRCMF_SCAN_STATUS_ABORT, &cfg->scan_status)) {
		bphy_err(wiphy, "Scanning being aborted: status (%lu)\n",
			 cfg->scan_status);
		return -EAGAIN;
	}
	if (test_bit(BRCMF_SCAN_STATUS_SUPPRESS, &cfg->scan_status)) {
		bphy_err(wiphy, "Scanning suppressed: status (%lu)\n",
			 cfg->scan_status);
		return -EAGAIN;
	}
	if (test_bit(BRCMF_VIF_STATUS_CONNECTING, &vif->sme_state)) {
		bphy_err(wiphy, "Connecting: status (%lu)\n", vif->sme_state);
		return -EAGAIN;
	}

	/* If scan req comes for p2p0, send it over primary I/F */
	if (vif == cfg->p2p.bss_idx[P2PAPI_BSSCFG_DEVICE].vif)
		vif = cfg->p2p.bss_idx[P2PAPI_BSSCFG_PRIMARY].vif;

	brcmf_dbg(SCAN, "START ESCAN\n");

	cfg->scan_request = request;
	set_bit(BRCMF_SCAN_STATUS_BUSY, &cfg->scan_status);

	cfg->escan_info.run = brcmf_run_escan;
	err = brcmf_p2p_scan_prep(wiphy, request, vif);
	if (err)
		goto scan_out;

	err = brcmf_vif_set_mgmt_ie(vif, BRCMF_VNDR_IE_PRBREQ_FLAG,
				    request->ie, request->ie_len);
	if (err)
		goto scan_out;

	err = brcmf_do_escan(vif->ifp, request);
	if (err)
		goto scan_out;

	/* Arm scan timeout timer */
	mod_timer(&cfg->escan_timeout,
		  jiffies + msecs_to_jiffies(BRCMF_ESCAN_TIMER_INTERVAL_MS));

	return 0;

scan_out:
	bphy_err(wiphy, "scan error (%d)\n", err);
	clear_bit(BRCMF_SCAN_STATUS_BUSY, &cfg->scan_status);
	cfg->scan_request = NULL;
	return err;
}