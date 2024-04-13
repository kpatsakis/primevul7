brcmf_cfg80211_update_conn_params(struct wiphy *wiphy,
				  struct net_device *ndev,
				  struct cfg80211_connect_params *sme,
				  u32 changed)
{
	struct brcmf_if *ifp;
	int err;

	if (!(changed & UPDATE_ASSOC_IES))
		return 0;

	ifp = netdev_priv(ndev);
	err = brcmf_vif_set_mgmt_ie(ifp->vif, BRCMF_VNDR_IE_ASSOCREQ_FLAG,
				    sme->ie, sme->ie_len);
	if (err)
		bphy_err(wiphy, "Set Assoc REQ IE Failed\n");
	else
		brcmf_dbg(TRACE, "Applied Vndr IEs for Assoc request\n");

	return err;
}