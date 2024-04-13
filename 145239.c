sendrequest(
	int opcode,
	associd_t associd,
	int auth,
	int qsize,
	const char *qdata
	)
{
	struct ntp_control qpkt;
	int	pktsize;
	u_long	key_id;
	char *	pass;
	int	maclen;

	/*
	 * Check to make sure the data will fit in one packet
	 */
	if (qsize > CTL_MAX_DATA_LEN) {
		fprintf(stderr,
			"***Internal error!  qsize (%d) too large\n",
			qsize);
		return 1;
	}

	/*
	 * Fill in the packet
	 */
	qpkt.li_vn_mode = PKT_LI_VN_MODE(0, pktversion, MODE_CONTROL);
	qpkt.r_m_e_op = (u_char)(opcode & CTL_OP_MASK);
	qpkt.sequence = htons(sequence);
	qpkt.status = 0;
	qpkt.associd = htons((u_short)associd);
	qpkt.offset = 0;
	qpkt.count = htons((u_short)qsize);

	pktsize = CTL_HEADER_LEN;

	/*
	 * If we have data, copy and pad it out to a 32-bit boundary.
	 */
	if (qsize > 0) {
		memcpy(&qpkt.u, qdata, (size_t)qsize);
		pktsize += qsize;
		while (pktsize & (sizeof(u_int32) - 1)) {
			qpkt.u.data[qsize++] = 0;
			pktsize++;
		}
	}

	/*
	 * If it isn't authenticated we can just send it.  Otherwise
	 * we're going to have to think about it a little.
	 */
	if (!auth && !always_auth) {
		return sendpkt(&qpkt, pktsize);
	}

	/*
	 * Pad out packet to a multiple of 8 octets to be sure
	 * receiver can handle it.
	 */
	while (pktsize & 7) {
		qpkt.u.data[qsize++] = 0;
		pktsize++;
	}

	/*
	 * Get the keyid and the password if we don't have one.
	 */
	if (info_auth_keyid == 0) {
		key_id = getkeyid("Keyid: ");
		if (key_id == 0 || key_id > NTP_MAXKEY) {
			fprintf(stderr,
				"Invalid key identifier\n");
			return 1;
		}
		info_auth_keyid = key_id;
	}
	if (!authistrusted(info_auth_keyid)) {
		pass = getpass_keytype(info_auth_keytype);
		if ('\0' == pass[0]) {
			fprintf(stderr, "Invalid password\n");
			return 1;
		}
		authusekey(info_auth_keyid, info_auth_keytype,
			   (u_char *)pass);
		authtrust(info_auth_keyid, 1);
	}

	/*
	 * Do the encryption.
	 */
	maclen = authencrypt(info_auth_keyid, (void *)&qpkt, pktsize);
	if (!maclen) {
		fprintf(stderr, "Key not found\n");
		return 1;
	} else if ((size_t)maclen != (info_auth_hashlen + sizeof(keyid_t))) {
		fprintf(stderr,
			"%d octet MAC, %zu expected with %zu octet digest\n",
			maclen, (info_auth_hashlen + sizeof(keyid_t)),
			info_auth_hashlen);
		return 1;
	}

	return sendpkt((char *)&qpkt, pktsize + maclen);
}