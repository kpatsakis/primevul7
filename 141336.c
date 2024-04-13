struct brcmf_cfg80211_vif *brcmf_alloc_vif(struct brcmf_cfg80211_info *cfg,
					   enum nl80211_iftype type)
{
	struct brcmf_cfg80211_vif *vif_walk;
	struct brcmf_cfg80211_vif *vif;
	bool mbss;

	brcmf_dbg(TRACE, "allocating virtual interface (size=%zu)\n",
		  sizeof(*vif));
	vif = kzalloc(sizeof(*vif), GFP_KERNEL);
	if (!vif)
		return ERR_PTR(-ENOMEM);

	vif->wdev.wiphy = cfg->wiphy;
	vif->wdev.iftype = type;

	brcmf_init_prof(&vif->profile);

	if (type == NL80211_IFTYPE_AP) {
		mbss = false;
		list_for_each_entry(vif_walk, &cfg->vif_list, list) {
			if (vif_walk->wdev.iftype == NL80211_IFTYPE_AP) {
				mbss = true;
				break;
			}
		}
		vif->mbss = mbss;
	}

	list_add_tail(&vif->list, &cfg->vif_list);
	return vif;
}