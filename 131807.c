static int ieee80211_get_keyid(struct sk_buff *skb,
			       const struct ieee80211_cipher_scheme *cs)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)skb->data;
	__le16 fc;
	int hdrlen;
	int minlen;
	u8 key_idx_off;
	u8 key_idx_shift;
	u8 keyid;

	fc = hdr->frame_control;
	hdrlen = ieee80211_hdrlen(fc);

	if (cs) {
		minlen = hdrlen + cs->hdr_len;
		key_idx_off = hdrlen + cs->key_idx_off;
		key_idx_shift = cs->key_idx_shift;
	} else {
		/* WEP, TKIP, CCMP and GCMP */
		minlen = hdrlen + IEEE80211_WEP_IV_LEN;
		key_idx_off = hdrlen + 3;
		key_idx_shift = 6;
	}

	if (unlikely(skb->len < minlen))
		return -EINVAL;

	skb_copy_bits(skb, key_idx_off, &keyid, 1);

	if (cs)
		keyid &= cs->key_idx_mask;
	keyid >>= key_idx_shift;

	/* cs could use more than the usual two bits for the keyid */
	if (unlikely(keyid >= NUM_DEFAULT_KEYS))
		return -EINVAL;

	return keyid;
}