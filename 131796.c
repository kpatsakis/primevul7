ieee80211_add_rx_radiotap_header(struct ieee80211_local *local,
				 struct sk_buff *skb,
				 struct ieee80211_rate *rate,
				 int rtap_len, bool has_fcs)
{
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
	struct ieee80211_radiotap_header *rthdr;
	unsigned char *pos;
	__le32 *it_present;
	u32 it_present_val;
	u16 rx_flags = 0;
	u16 channel_flags = 0;
	int mpdulen, chain;
	unsigned long chains = status->chains;
	struct ieee80211_vendor_radiotap rtap = {};
	struct ieee80211_radiotap_he he = {};
	struct ieee80211_radiotap_he_mu he_mu = {};
	struct ieee80211_radiotap_lsig lsig = {};

	if (status->flag & RX_FLAG_RADIOTAP_HE) {
		he = *(struct ieee80211_radiotap_he *)skb->data;
		skb_pull(skb, sizeof(he));
		WARN_ON_ONCE(status->encoding != RX_ENC_HE);
	}

	if (status->flag & RX_FLAG_RADIOTAP_HE_MU) {
		he_mu = *(struct ieee80211_radiotap_he_mu *)skb->data;
		skb_pull(skb, sizeof(he_mu));
	}

	if (status->flag & RX_FLAG_RADIOTAP_LSIG) {
		lsig = *(struct ieee80211_radiotap_lsig *)skb->data;
		skb_pull(skb, sizeof(lsig));
	}

	if (status->flag & RX_FLAG_RADIOTAP_VENDOR_DATA) {
		rtap = *(struct ieee80211_vendor_radiotap *)skb->data;
		/* rtap.len and rtap.pad are undone immediately */
		skb_pull(skb, sizeof(rtap) + rtap.len + rtap.pad);
	}

	mpdulen = skb->len;
	if (!(has_fcs && ieee80211_hw_check(&local->hw, RX_INCLUDES_FCS)))
		mpdulen += FCS_LEN;

	rthdr = skb_push(skb, rtap_len);
	memset(rthdr, 0, rtap_len - rtap.len - rtap.pad);
	it_present = &rthdr->it_present;

	/* radiotap header, set always present flags */
	rthdr->it_len = cpu_to_le16(rtap_len);
	it_present_val = BIT(IEEE80211_RADIOTAP_FLAGS) |
			 BIT(IEEE80211_RADIOTAP_CHANNEL) |
			 BIT(IEEE80211_RADIOTAP_RX_FLAGS);

	if (!status->chains)
		it_present_val |= BIT(IEEE80211_RADIOTAP_ANTENNA);

	for_each_set_bit(chain, &chains, IEEE80211_MAX_CHAINS) {
		it_present_val |=
			BIT(IEEE80211_RADIOTAP_EXT) |
			BIT(IEEE80211_RADIOTAP_RADIOTAP_NAMESPACE);
		put_unaligned_le32(it_present_val, it_present);
		it_present++;
		it_present_val = BIT(IEEE80211_RADIOTAP_ANTENNA) |
				 BIT(IEEE80211_RADIOTAP_DBM_ANTSIGNAL);
	}

	if (status->flag & RX_FLAG_RADIOTAP_VENDOR_DATA) {
		it_present_val |= BIT(IEEE80211_RADIOTAP_VENDOR_NAMESPACE) |
				  BIT(IEEE80211_RADIOTAP_EXT);
		put_unaligned_le32(it_present_val, it_present);
		it_present++;
		it_present_val = rtap.present;
	}

	put_unaligned_le32(it_present_val, it_present);

	pos = (void *)(it_present + 1);

	/* the order of the following fields is important */

	/* IEEE80211_RADIOTAP_TSFT */
	if (ieee80211_have_rx_timestamp(status)) {
		/* padding */
		while ((pos - (u8 *)rthdr) & 7)
			*pos++ = 0;
		put_unaligned_le64(
			ieee80211_calculate_rx_timestamp(local, status,
							 mpdulen, 0),
			pos);
		rthdr->it_present |= cpu_to_le32(1 << IEEE80211_RADIOTAP_TSFT);
		pos += 8;
	}

	/* IEEE80211_RADIOTAP_FLAGS */
	if (has_fcs && ieee80211_hw_check(&local->hw, RX_INCLUDES_FCS))
		*pos |= IEEE80211_RADIOTAP_F_FCS;
	if (status->flag & (RX_FLAG_FAILED_FCS_CRC | RX_FLAG_FAILED_PLCP_CRC))
		*pos |= IEEE80211_RADIOTAP_F_BADFCS;
	if (status->enc_flags & RX_ENC_FLAG_SHORTPRE)
		*pos |= IEEE80211_RADIOTAP_F_SHORTPRE;
	pos++;

	/* IEEE80211_RADIOTAP_RATE */
	if (!rate || status->encoding != RX_ENC_LEGACY) {
		/*
		 * Without rate information don't add it. If we have,
		 * MCS information is a separate field in radiotap,
		 * added below. The byte here is needed as padding
		 * for the channel though, so initialise it to 0.
		 */
		*pos = 0;
	} else {
		int shift = 0;
		rthdr->it_present |= cpu_to_le32(1 << IEEE80211_RADIOTAP_RATE);
		if (status->bw == RATE_INFO_BW_10)
			shift = 1;
		else if (status->bw == RATE_INFO_BW_5)
			shift = 2;
		*pos = DIV_ROUND_UP(rate->bitrate, 5 * (1 << shift));
	}
	pos++;

	/* IEEE80211_RADIOTAP_CHANNEL */
	put_unaligned_le16(status->freq, pos);
	pos += 2;
	if (status->bw == RATE_INFO_BW_10)
		channel_flags |= IEEE80211_CHAN_HALF;
	else if (status->bw == RATE_INFO_BW_5)
		channel_flags |= IEEE80211_CHAN_QUARTER;

	if (status->band == NL80211_BAND_5GHZ)
		channel_flags |= IEEE80211_CHAN_OFDM | IEEE80211_CHAN_5GHZ;
	else if (status->encoding != RX_ENC_LEGACY)
		channel_flags |= IEEE80211_CHAN_DYN | IEEE80211_CHAN_2GHZ;
	else if (rate && rate->flags & IEEE80211_RATE_ERP_G)
		channel_flags |= IEEE80211_CHAN_OFDM | IEEE80211_CHAN_2GHZ;
	else if (rate)
		channel_flags |= IEEE80211_CHAN_CCK | IEEE80211_CHAN_2GHZ;
	else
		channel_flags |= IEEE80211_CHAN_2GHZ;
	put_unaligned_le16(channel_flags, pos);
	pos += 2;

	/* IEEE80211_RADIOTAP_DBM_ANTSIGNAL */
	if (ieee80211_hw_check(&local->hw, SIGNAL_DBM) &&
	    !(status->flag & RX_FLAG_NO_SIGNAL_VAL)) {
		*pos = status->signal;
		rthdr->it_present |=
			cpu_to_le32(1 << IEEE80211_RADIOTAP_DBM_ANTSIGNAL);
		pos++;
	}

	/* IEEE80211_RADIOTAP_LOCK_QUALITY is missing */

	if (!status->chains) {
		/* IEEE80211_RADIOTAP_ANTENNA */
		*pos = status->antenna;
		pos++;
	}

	/* IEEE80211_RADIOTAP_DB_ANTNOISE is not used */

	/* IEEE80211_RADIOTAP_RX_FLAGS */
	/* ensure 2 byte alignment for the 2 byte field as required */
	if ((pos - (u8 *)rthdr) & 1)
		*pos++ = 0;
	if (status->flag & RX_FLAG_FAILED_PLCP_CRC)
		rx_flags |= IEEE80211_RADIOTAP_F_RX_BADPLCP;
	put_unaligned_le16(rx_flags, pos);
	pos += 2;

	if (status->encoding == RX_ENC_HT) {
		unsigned int stbc;

		rthdr->it_present |= cpu_to_le32(1 << IEEE80211_RADIOTAP_MCS);
		*pos++ = local->hw.radiotap_mcs_details;
		*pos = 0;
		if (status->enc_flags & RX_ENC_FLAG_SHORT_GI)
			*pos |= IEEE80211_RADIOTAP_MCS_SGI;
		if (status->bw == RATE_INFO_BW_40)
			*pos |= IEEE80211_RADIOTAP_MCS_BW_40;
		if (status->enc_flags & RX_ENC_FLAG_HT_GF)
			*pos |= IEEE80211_RADIOTAP_MCS_FMT_GF;
		if (status->enc_flags & RX_ENC_FLAG_LDPC)
			*pos |= IEEE80211_RADIOTAP_MCS_FEC_LDPC;
		stbc = (status->enc_flags & RX_ENC_FLAG_STBC_MASK) >> RX_ENC_FLAG_STBC_SHIFT;
		*pos |= stbc << IEEE80211_RADIOTAP_MCS_STBC_SHIFT;
		pos++;
		*pos++ = status->rate_idx;
	}

	if (status->flag & RX_FLAG_AMPDU_DETAILS) {
		u16 flags = 0;

		/* ensure 4 byte alignment */
		while ((pos - (u8 *)rthdr) & 3)
			pos++;
		rthdr->it_present |=
			cpu_to_le32(1 << IEEE80211_RADIOTAP_AMPDU_STATUS);
		put_unaligned_le32(status->ampdu_reference, pos);
		pos += 4;
		if (status->flag & RX_FLAG_AMPDU_LAST_KNOWN)
			flags |= IEEE80211_RADIOTAP_AMPDU_LAST_KNOWN;
		if (status->flag & RX_FLAG_AMPDU_IS_LAST)
			flags |= IEEE80211_RADIOTAP_AMPDU_IS_LAST;
		if (status->flag & RX_FLAG_AMPDU_DELIM_CRC_ERROR)
			flags |= IEEE80211_RADIOTAP_AMPDU_DELIM_CRC_ERR;
		if (status->flag & RX_FLAG_AMPDU_DELIM_CRC_KNOWN)
			flags |= IEEE80211_RADIOTAP_AMPDU_DELIM_CRC_KNOWN;
		if (status->flag & RX_FLAG_AMPDU_EOF_BIT_KNOWN)
			flags |= IEEE80211_RADIOTAP_AMPDU_EOF_KNOWN;
		if (status->flag & RX_FLAG_AMPDU_EOF_BIT)
			flags |= IEEE80211_RADIOTAP_AMPDU_EOF;
		put_unaligned_le16(flags, pos);
		pos += 2;
		if (status->flag & RX_FLAG_AMPDU_DELIM_CRC_KNOWN)
			*pos++ = status->ampdu_delimiter_crc;
		else
			*pos++ = 0;
		*pos++ = 0;
	}

	if (status->encoding == RX_ENC_VHT) {
		u16 known = local->hw.radiotap_vht_details;

		rthdr->it_present |= cpu_to_le32(1 << IEEE80211_RADIOTAP_VHT);
		put_unaligned_le16(known, pos);
		pos += 2;
		/* flags */
		if (status->enc_flags & RX_ENC_FLAG_SHORT_GI)
			*pos |= IEEE80211_RADIOTAP_VHT_FLAG_SGI;
		/* in VHT, STBC is binary */
		if (status->enc_flags & RX_ENC_FLAG_STBC_MASK)
			*pos |= IEEE80211_RADIOTAP_VHT_FLAG_STBC;
		if (status->enc_flags & RX_ENC_FLAG_BF)
			*pos |= IEEE80211_RADIOTAP_VHT_FLAG_BEAMFORMED;
		pos++;
		/* bandwidth */
		switch (status->bw) {
		case RATE_INFO_BW_80:
			*pos++ = 4;
			break;
		case RATE_INFO_BW_160:
			*pos++ = 11;
			break;
		case RATE_INFO_BW_40:
			*pos++ = 1;
			break;
		default:
			*pos++ = 0;
		}
		/* MCS/NSS */
		*pos = (status->rate_idx << 4) | status->nss;
		pos += 4;
		/* coding field */
		if (status->enc_flags & RX_ENC_FLAG_LDPC)
			*pos |= IEEE80211_RADIOTAP_CODING_LDPC_USER0;
		pos++;
		/* group ID */
		pos++;
		/* partial_aid */
		pos += 2;
	}

	if (local->hw.radiotap_timestamp.units_pos >= 0) {
		u16 accuracy = 0;
		u8 flags = IEEE80211_RADIOTAP_TIMESTAMP_FLAG_32BIT;

		rthdr->it_present |=
			cpu_to_le32(1 << IEEE80211_RADIOTAP_TIMESTAMP);

		/* ensure 8 byte alignment */
		while ((pos - (u8 *)rthdr) & 7)
			pos++;

		put_unaligned_le64(status->device_timestamp, pos);
		pos += sizeof(u64);

		if (local->hw.radiotap_timestamp.accuracy >= 0) {
			accuracy = local->hw.radiotap_timestamp.accuracy;
			flags |= IEEE80211_RADIOTAP_TIMESTAMP_FLAG_ACCURACY;
		}
		put_unaligned_le16(accuracy, pos);
		pos += sizeof(u16);

		*pos++ = local->hw.radiotap_timestamp.units_pos;
		*pos++ = flags;
	}

	if (status->encoding == RX_ENC_HE &&
	    status->flag & RX_FLAG_RADIOTAP_HE) {
#define HE_PREP(f, val)	le16_encode_bits(val, IEEE80211_RADIOTAP_HE_##f)

		if (status->enc_flags & RX_ENC_FLAG_STBC_MASK) {
			he.data6 |= HE_PREP(DATA6_NSTS,
					    FIELD_GET(RX_ENC_FLAG_STBC_MASK,
						      status->enc_flags));
			he.data3 |= HE_PREP(DATA3_STBC, 1);
		} else {
			he.data6 |= HE_PREP(DATA6_NSTS, status->nss);
		}

#define CHECK_GI(s) \
	BUILD_BUG_ON(IEEE80211_RADIOTAP_HE_DATA5_GI_##s != \
		     (int)NL80211_RATE_INFO_HE_GI_##s)

		CHECK_GI(0_8);
		CHECK_GI(1_6);
		CHECK_GI(3_2);

		he.data3 |= HE_PREP(DATA3_DATA_MCS, status->rate_idx);
		he.data3 |= HE_PREP(DATA3_DATA_DCM, status->he_dcm);
		he.data3 |= HE_PREP(DATA3_CODING,
				    !!(status->enc_flags & RX_ENC_FLAG_LDPC));

		he.data5 |= HE_PREP(DATA5_GI, status->he_gi);

		switch (status->bw) {
		case RATE_INFO_BW_20:
			he.data5 |= HE_PREP(DATA5_DATA_BW_RU_ALLOC,
					    IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC_20MHZ);
			break;
		case RATE_INFO_BW_40:
			he.data5 |= HE_PREP(DATA5_DATA_BW_RU_ALLOC,
					    IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC_40MHZ);
			break;
		case RATE_INFO_BW_80:
			he.data5 |= HE_PREP(DATA5_DATA_BW_RU_ALLOC,
					    IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC_80MHZ);
			break;
		case RATE_INFO_BW_160:
			he.data5 |= HE_PREP(DATA5_DATA_BW_RU_ALLOC,
					    IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC_160MHZ);
			break;
		case RATE_INFO_BW_HE_RU:
#define CHECK_RU_ALLOC(s) \
	BUILD_BUG_ON(IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC_##s##T != \
		     NL80211_RATE_INFO_HE_RU_ALLOC_##s + 4)

			CHECK_RU_ALLOC(26);
			CHECK_RU_ALLOC(52);
			CHECK_RU_ALLOC(106);
			CHECK_RU_ALLOC(242);
			CHECK_RU_ALLOC(484);
			CHECK_RU_ALLOC(996);
			CHECK_RU_ALLOC(2x996);

			he.data5 |= HE_PREP(DATA5_DATA_BW_RU_ALLOC,
					    status->he_ru + 4);
			break;
		default:
			WARN_ONCE(1, "Invalid SU BW %d\n", status->bw);
		}

		/* ensure 2 byte alignment */
		while ((pos - (u8 *)rthdr) & 1)
			pos++;
		rthdr->it_present |= cpu_to_le32(1 << IEEE80211_RADIOTAP_HE);
		memcpy(pos, &he, sizeof(he));
		pos += sizeof(he);
	}

	if (status->encoding == RX_ENC_HE &&
	    status->flag & RX_FLAG_RADIOTAP_HE_MU) {
		/* ensure 2 byte alignment */
		while ((pos - (u8 *)rthdr) & 1)
			pos++;
		rthdr->it_present |= cpu_to_le32(1 << IEEE80211_RADIOTAP_HE_MU);
		memcpy(pos, &he_mu, sizeof(he_mu));
		pos += sizeof(he_mu);
	}

	if (status->flag & RX_FLAG_NO_PSDU) {
		rthdr->it_present |=
			cpu_to_le32(1 << IEEE80211_RADIOTAP_ZERO_LEN_PSDU);
		*pos++ = status->zero_length_psdu_type;
	}

	if (status->flag & RX_FLAG_RADIOTAP_LSIG) {
		/* ensure 2 byte alignment */
		while ((pos - (u8 *)rthdr) & 1)
			pos++;
		rthdr->it_present |= cpu_to_le32(1 << IEEE80211_RADIOTAP_LSIG);
		memcpy(pos, &lsig, sizeof(lsig));
		pos += sizeof(lsig);
	}

	for_each_set_bit(chain, &chains, IEEE80211_MAX_CHAINS) {
		*pos++ = status->chain_signal[chain];
		*pos++ = chain;
	}

	if (status->flag & RX_FLAG_RADIOTAP_VENDOR_DATA) {
		/* ensure 2 byte alignment for the vendor field as required */
		if ((pos - (u8 *)rthdr) & 1)
			*pos++ = 0;
		*pos++ = rtap.oui[0];
		*pos++ = rtap.oui[1];
		*pos++ = rtap.oui[2];
		*pos++ = rtap.subns;
		put_unaligned_le16(rtap.len, pos);
		pos += 2;
		/* align the actual payload as requested */
		while ((pos - (u8 *)rthdr) & (rtap.align - 1))
			*pos++ = 0;
		/* data (and possible padding) already follows */
	}
}