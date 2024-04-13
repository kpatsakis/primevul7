ieee80211_rx_h_decrypt(struct ieee80211_rx_data *rx)
{
	struct sk_buff *skb = rx->skb;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)skb->data;
	int keyidx;
	ieee80211_rx_result result = RX_DROP_UNUSABLE;
	struct ieee80211_key *sta_ptk = NULL;
	struct ieee80211_key *ptk_idx = NULL;
	int mmie_keyidx = -1;
	__le16 fc;
	const struct ieee80211_cipher_scheme *cs = NULL;

	/*
	 * Key selection 101
	 *
	 * There are four types of keys:
	 *  - GTK (group keys)
	 *  - IGTK (group keys for management frames)
	 *  - PTK (pairwise keys)
	 *  - STK (station-to-station pairwise keys)
	 *
	 * When selecting a key, we have to distinguish between multicast
	 * (including broadcast) and unicast frames, the latter can only
	 * use PTKs and STKs while the former always use GTKs and IGTKs.
	 * Unless, of course, actual WEP keys ("pre-RSNA") are used, then
	 * unicast frames can also use key indices like GTKs. Hence, if we
	 * don't have a PTK/STK we check the key index for a WEP key.
	 *
	 * Note that in a regular BSS, multicast frames are sent by the
	 * AP only, associated stations unicast the frame to the AP first
	 * which then multicasts it on their behalf.
	 *
	 * There is also a slight problem in IBSS mode: GTKs are negotiated
	 * with each station, that is something we don't currently handle.
	 * The spec seems to expect that one negotiates the same key with
	 * every station but there's no such requirement; VLANs could be
	 * possible.
	 */

	/* start without a key */
	rx->key = NULL;
	fc = hdr->frame_control;

	if (rx->sta) {
		int keyid = rx->sta->ptk_idx;
		sta_ptk = rcu_dereference(rx->sta->ptk[keyid]);

		if (ieee80211_has_protected(fc)) {
			cs = rx->sta->cipher_scheme;
			keyid = ieee80211_get_keyid(rx->skb, cs);

			if (unlikely(keyid < 0))
				return RX_DROP_UNUSABLE;

			ptk_idx = rcu_dereference(rx->sta->ptk[keyid]);
		}
	}

	if (!ieee80211_has_protected(fc))
		mmie_keyidx = ieee80211_get_mmie_keyidx(rx->skb);

	if (!is_multicast_ether_addr(hdr->addr1) && sta_ptk) {
		rx->key = ptk_idx ? ptk_idx : sta_ptk;
		if ((status->flag & RX_FLAG_DECRYPTED) &&
		    (status->flag & RX_FLAG_IV_STRIPPED))
			return RX_CONTINUE;
		/* Skip decryption if the frame is not protected. */
		if (!ieee80211_has_protected(fc))
			return RX_CONTINUE;
	} else if (mmie_keyidx >= 0) {
		/* Broadcast/multicast robust management frame / BIP */
		if ((status->flag & RX_FLAG_DECRYPTED) &&
		    (status->flag & RX_FLAG_IV_STRIPPED))
			return RX_CONTINUE;

		if (mmie_keyidx < NUM_DEFAULT_KEYS ||
		    mmie_keyidx >= NUM_DEFAULT_KEYS + NUM_DEFAULT_MGMT_KEYS)
			return RX_DROP_MONITOR; /* unexpected BIP keyidx */
		if (rx->sta) {
			if (ieee80211_is_group_privacy_action(skb) &&
			    test_sta_flag(rx->sta, WLAN_STA_MFP))
				return RX_DROP_MONITOR;

			rx->key = rcu_dereference(rx->sta->gtk[mmie_keyidx]);
		}
		if (!rx->key)
			rx->key = rcu_dereference(rx->sdata->keys[mmie_keyidx]);
	} else if (!ieee80211_has_protected(fc)) {
		/*
		 * The frame was not protected, so skip decryption. However, we
		 * need to set rx->key if there is a key that could have been
		 * used so that the frame may be dropped if encryption would
		 * have been expected.
		 */
		struct ieee80211_key *key = NULL;
		struct ieee80211_sub_if_data *sdata = rx->sdata;
		int i;

		if (ieee80211_is_mgmt(fc) &&
		    is_multicast_ether_addr(hdr->addr1) &&
		    (key = rcu_dereference(rx->sdata->default_mgmt_key)))
			rx->key = key;
		else {
			if (rx->sta) {
				for (i = 0; i < NUM_DEFAULT_KEYS; i++) {
					key = rcu_dereference(rx->sta->gtk[i]);
					if (key)
						break;
				}
			}
			if (!key) {
				for (i = 0; i < NUM_DEFAULT_KEYS; i++) {
					key = rcu_dereference(sdata->keys[i]);
					if (key)
						break;
				}
			}
			if (key)
				rx->key = key;
		}
		return RX_CONTINUE;
	} else {
		/*
		 * The device doesn't give us the IV so we won't be
		 * able to look up the key. That's ok though, we
		 * don't need to decrypt the frame, we just won't
		 * be able to keep statistics accurate.
		 * Except for key threshold notifications, should
		 * we somehow allow the driver to tell us which key
		 * the hardware used if this flag is set?
		 */
		if ((status->flag & RX_FLAG_DECRYPTED) &&
		    (status->flag & RX_FLAG_IV_STRIPPED))
			return RX_CONTINUE;

		keyidx = ieee80211_get_keyid(rx->skb, cs);

		if (unlikely(keyidx < 0))
			return RX_DROP_UNUSABLE;

		/* check per-station GTK first, if multicast packet */
		if (is_multicast_ether_addr(hdr->addr1) && rx->sta)
			rx->key = rcu_dereference(rx->sta->gtk[keyidx]);

		/* if not found, try default key */
		if (!rx->key) {
			rx->key = rcu_dereference(rx->sdata->keys[keyidx]);

			/*
			 * RSNA-protected unicast frames should always be
			 * sent with pairwise or station-to-station keys,
			 * but for WEP we allow using a key index as well.
			 */
			if (rx->key &&
			    rx->key->conf.cipher != WLAN_CIPHER_SUITE_WEP40 &&
			    rx->key->conf.cipher != WLAN_CIPHER_SUITE_WEP104 &&
			    !is_multicast_ether_addr(hdr->addr1))
				rx->key = NULL;
		}
	}

	if (rx->key) {
		if (unlikely(rx->key->flags & KEY_FLAG_TAINTED))
			return RX_DROP_MONITOR;

		/* TODO: add threshold stuff again */
	} else {
		return RX_DROP_MONITOR;
	}

	switch (rx->key->conf.cipher) {
	case WLAN_CIPHER_SUITE_WEP40:
	case WLAN_CIPHER_SUITE_WEP104:
		result = ieee80211_crypto_wep_decrypt(rx);
		break;
	case WLAN_CIPHER_SUITE_TKIP:
		result = ieee80211_crypto_tkip_decrypt(rx);
		break;
	case WLAN_CIPHER_SUITE_CCMP:
		result = ieee80211_crypto_ccmp_decrypt(
			rx, IEEE80211_CCMP_MIC_LEN);
		break;
	case WLAN_CIPHER_SUITE_CCMP_256:
		result = ieee80211_crypto_ccmp_decrypt(
			rx, IEEE80211_CCMP_256_MIC_LEN);
		break;
	case WLAN_CIPHER_SUITE_AES_CMAC:
		result = ieee80211_crypto_aes_cmac_decrypt(rx);
		break;
	case WLAN_CIPHER_SUITE_BIP_CMAC_256:
		result = ieee80211_crypto_aes_cmac_256_decrypt(rx);
		break;
	case WLAN_CIPHER_SUITE_BIP_GMAC_128:
	case WLAN_CIPHER_SUITE_BIP_GMAC_256:
		result = ieee80211_crypto_aes_gmac_decrypt(rx);
		break;
	case WLAN_CIPHER_SUITE_GCMP:
	case WLAN_CIPHER_SUITE_GCMP_256:
		result = ieee80211_crypto_gcmp_decrypt(rx);
		break;
	default:
		result = ieee80211_crypto_hw_decrypt(rx);
	}

	/* the hdr variable is invalid after the decrypt handlers */

	/* either the frame has been decrypted or will be dropped */
	status->flag |= RX_FLAG_DECRYPTED;

	return result;
}