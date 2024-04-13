static int brcmf_vif_change_validate(struct brcmf_cfg80211_info *cfg,
				     struct brcmf_cfg80211_vif *vif,
				     enum nl80211_iftype new_type)
{
	struct brcmf_cfg80211_vif *pos;
	bool check_combos = false;
	int ret = 0;
	struct iface_combination_params params = {
		.num_different_channels = 1,
	};

	list_for_each_entry(pos, &cfg->vif_list, list)
		if (pos == vif) {
			params.iftype_num[new_type]++;
		} else {
			/* concurrent interfaces so need check combinations */
			check_combos = true;
			params.iftype_num[pos->wdev.iftype]++;
		}

	if (check_combos)
		ret = cfg80211_check_combinations(cfg->wiphy, &params);

	return ret;
}