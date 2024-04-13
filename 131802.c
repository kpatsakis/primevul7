ieee80211_rx_radiotap_hdrlen(struct ieee80211_local *local,
			     struct ieee80211_rx_status *status,
			     struct sk_buff *skb)
{
	int len;

	/* always present fields */
	len = sizeof(struct ieee80211_radiotap_header) + 8;

	/* allocate extra bitmaps */
	if (status->chains)
		len += 4 * hweight8(status->chains);
	/* vendor presence bitmap */
	if (status->flag & RX_FLAG_RADIOTAP_VENDOR_DATA)
		len += 4;

	if (ieee80211_have_rx_timestamp(status)) {
		len = ALIGN(len, 8);
		len += 8;
	}
	if (ieee80211_hw_check(&local->hw, SIGNAL_DBM))
		len += 1;

	/* antenna field, if we don't have per-chain info */
	if (!status->chains)
		len += 1;

	/* padding for RX_FLAGS if necessary */
	len = ALIGN(len, 2);

	if (status->encoding == RX_ENC_HT) /* HT info */
		len += 3;

	if (status->flag & RX_FLAG_AMPDU_DETAILS) {
		len = ALIGN(len, 4);
		len += 8;
	}

	if (status->encoding == RX_ENC_VHT) {
		len = ALIGN(len, 2);
		len += 12;
	}

	if (local->hw.radiotap_timestamp.units_pos >= 0) {
		len = ALIGN(len, 8);
		len += 12;
	}

	if (status->encoding == RX_ENC_HE &&
	    status->flag & RX_FLAG_RADIOTAP_HE) {
		len = ALIGN(len, 2);
		len += 12;
		BUILD_BUG_ON(sizeof(struct ieee80211_radiotap_he) != 12);
	}

	if (status->encoding == RX_ENC_HE &&
	    status->flag & RX_FLAG_RADIOTAP_HE_MU) {
		len = ALIGN(len, 2);
		len += 12;
		BUILD_BUG_ON(sizeof(struct ieee80211_radiotap_he_mu) != 12);
	}

	if (status->flag & RX_FLAG_NO_PSDU)
		len += 1;

	if (status->flag & RX_FLAG_RADIOTAP_LSIG) {
		len = ALIGN(len, 2);
		len += 4;
		BUILD_BUG_ON(sizeof(struct ieee80211_radiotap_lsig) != 4);
	}

	if (status->chains) {
		/* antenna and antenna signal fields */
		len += 2 * hweight8(status->chains);
	}

	if (status->flag & RX_FLAG_RADIOTAP_VENDOR_DATA) {
		struct ieee80211_vendor_radiotap *rtap;
		int vendor_data_offset = 0;

		/*
		 * The position to look at depends on the existence (or non-
		 * existence) of other elements, so take that into account...
		 */
		if (status->flag & RX_FLAG_RADIOTAP_HE)
			vendor_data_offset +=
				sizeof(struct ieee80211_radiotap_he);
		if (status->flag & RX_FLAG_RADIOTAP_HE_MU)
			vendor_data_offset +=
				sizeof(struct ieee80211_radiotap_he_mu);
		if (status->flag & RX_FLAG_RADIOTAP_LSIG)
			vendor_data_offset +=
				sizeof(struct ieee80211_radiotap_lsig);

		rtap = (void *)&skb->data[vendor_data_offset];

		/* alignment for fixed 6-byte vendor data header */
		len = ALIGN(len, 2);
		/* vendor data header */
		len += 6;
		if (WARN_ON(rtap->align == 0))
			rtap->align = 1;
		len = ALIGN(len, rtap->align);
		len += rtap->len + rtap->pad;
	}

	return len;
}