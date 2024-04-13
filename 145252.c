getresponse(
	int opcode,
	int associd,
	u_short *rstatus,
	int *rsize,
	const char **rdata,
	int timeo
	)
{
	struct ntp_control rpkt;
	struct sock_timeval tvo;
	u_short offsets[MAXFRAGS+1];
	u_short counts[MAXFRAGS+1];
	u_short offset;
	u_short count;
	size_t numfrags;
	size_t f;
	size_t ff;
	int seenlastfrag;
	int shouldbesize;
	fd_set fds;
	int n;
	int errcode;

	/*
	 * This is pretty tricky.  We may get between 1 and MAXFRAG packets
	 * back in response to the request.  We peel the data out of
	 * each packet and collect it in one long block.  When the last
	 * packet in the sequence is received we'll know how much data we
	 * should have had.  Note we use one long time out, should reconsider.
	 */
	*rsize = 0;
	if (rstatus)
		*rstatus = 0;
	*rdata = (char *)pktdata;

	numfrags = 0;
	seenlastfrag = 0;

	FD_ZERO(&fds);

	/*
	 * Loop until we have an error or a complete response.  Nearly all
	 * code paths to loop again use continue.
	 */
	for (;;) {

		if (numfrags == 0)
			tvo = tvout;
		else
			tvo = tvsout;

		FD_SET(sockfd, &fds);
		n = select(sockfd + 1, &fds, NULL, NULL, &tvo);

		if (n == -1) {
			warning("select fails");
			return -1;
		}
		if (n == 0) {
			/*
			 * Timed out.  Return what we have
			 */
			if (numfrags == 0) {
				if (timeo)
					fprintf(stderr,
						"%s: timed out, nothing received\n",
						currenthost);
				return ERR_TIMEOUT;
			}
			if (timeo)
				fprintf(stderr,
					"%s: timed out with incomplete data\n",
					currenthost);
			if (debug) {
				fprintf(stderr,
					"ERR_INCOMPLETE: Received fragments:\n");
				for (f = 0; f < numfrags; f++)
					fprintf(stderr,
						"%2u: %5d %5d\t%3d octets\n",
						(u_int)f, offsets[f],
						offsets[f] +
						counts[f],
						counts[f]);
				fprintf(stderr,
					"last fragment %sreceived\n",
					(seenlastfrag)
					    ? ""
					    : "not ");
			}
			return ERR_INCOMPLETE;
		}

		n = recv(sockfd, (char *)&rpkt, sizeof(rpkt), 0);
		if (n == -1) {
			warning("read");
			return -1;
		}

		if (debug >= 4) {
			printf("Response packet:\n");
			dump_hex_printable(&rpkt, n);
		}

		/*
		 * Check for format errors.  Bug proofing.
		 */
		if (n < (int)CTL_HEADER_LEN) {
			if (debug)
				printf("Short (%d byte) packet received\n", n);
			continue;
		}
		if (PKT_VERSION(rpkt.li_vn_mode) > NTP_VERSION
		    || PKT_VERSION(rpkt.li_vn_mode) < NTP_OLDVERSION) {
			if (debug)
				printf("Packet received with version %d\n",
				       PKT_VERSION(rpkt.li_vn_mode));
			continue;
		}
		if (PKT_MODE(rpkt.li_vn_mode) != MODE_CONTROL) {
			if (debug)
				printf("Packet received with mode %d\n",
				       PKT_MODE(rpkt.li_vn_mode));
			continue;
		}
		if (!CTL_ISRESPONSE(rpkt.r_m_e_op)) {
			if (debug)
				printf("Received request packet, wanted response\n");
			continue;
		}

		/*
		 * Check opcode and sequence number for a match.
		 * Could be old data getting to us.
		 */
		if (ntohs(rpkt.sequence) != sequence) {
			if (debug)
				printf("Received sequnce number %d, wanted %d\n",
				       ntohs(rpkt.sequence), sequence);
			continue;
		}
		if (CTL_OP(rpkt.r_m_e_op) != opcode) {
			if (debug)
			    printf(
				    "Received opcode %d, wanted %d (sequence number okay)\n",
				    CTL_OP(rpkt.r_m_e_op), opcode);
			continue;
		}

		/*
		 * Check the error code.  If non-zero, return it.
		 */
		if (CTL_ISERROR(rpkt.r_m_e_op)) {
			errcode = (ntohs(rpkt.status) >> 8) & 0xff;
			if (CTL_ISMORE(rpkt.r_m_e_op))
				TRACE(1, ("Error code %d received on not-final packet\n",
					  errcode));
			if (errcode == CERR_UNSPEC)
				return ERR_UNSPEC;
			return errcode;
		}

		/*
		 * Check the association ID to make sure it matches what
		 * we sent.
		 */
		if (ntohs(rpkt.associd) != associd) {
			TRACE(1, ("Association ID %d doesn't match expected %d\n",
				  ntohs(rpkt.associd), associd));
			/*
			 * Hack for silly fuzzballs which, at the time of writing,
			 * return an assID of sys.peer when queried for system variables.
			 */
#ifdef notdef
			continue;
#endif
		}

		/*
		 * Collect offset and count.  Make sure they make sense.
		 */
		offset = ntohs(rpkt.offset);
		count = ntohs(rpkt.count);

		/*
		 * validate received payload size is padded to next 32-bit
		 * boundary and no smaller than claimed by rpkt.count
		 */
		if (n & 0x3) {
			TRACE(1, ("Response packet not padded, size = %d\n",
				  n));
			continue;
		}

		shouldbesize = (CTL_HEADER_LEN + count + 3) & ~3;

		if (n < shouldbesize) {
			printf("Response packet claims %u octets payload, above %ld received\n",
			       count, (long)n - CTL_HEADER_LEN);
			return ERR_INCOMPLETE;
		}

		if (debug >= 3 && shouldbesize > n) {
			u_int32 key;
			u_int32 *lpkt;
			int maclen;

			/*
			 * Usually we ignore authentication, but for debugging purposes
			 * we watch it here.
			 */
			/* round to 8 octet boundary */
			shouldbesize = (shouldbesize + 7) & ~7;

			maclen = n - shouldbesize;
			if (maclen >= (int)MIN_MAC_LEN) {
				printf(
					"Packet shows signs of authentication (total %d, data %d, mac %d)\n",
					n, shouldbesize, maclen);
				lpkt = (u_int32 *)&rpkt;
				printf("%08lx %08lx %08lx %08lx %08lx %08lx\n",
				       (u_long)ntohl(lpkt[(n - maclen)/sizeof(u_int32) - 3]),
				       (u_long)ntohl(lpkt[(n - maclen)/sizeof(u_int32) - 2]),
				       (u_long)ntohl(lpkt[(n - maclen)/sizeof(u_int32) - 1]),
				       (u_long)ntohl(lpkt[(n - maclen)/sizeof(u_int32)]),
				       (u_long)ntohl(lpkt[(n - maclen)/sizeof(u_int32) + 1]),
				       (u_long)ntohl(lpkt[(n - maclen)/sizeof(u_int32) + 2]));
				key = ntohl(lpkt[(n - maclen) / sizeof(u_int32)]);
				printf("Authenticated with keyid %lu\n", (u_long)key);
				if (key != 0 && key != info_auth_keyid) {
					printf("We don't know that key\n");
				} else {
					if (authdecrypt(key, (u_int32 *)&rpkt,
					    n - maclen, maclen)) {
						printf("Auth okay!\n");
					} else {
						printf("Auth failed!\n");
					}
				}
			}
		}

		TRACE(2, ("Got packet, size = %d\n", n));
		if (count > (n - CTL_HEADER_LEN)) {
			TRACE(1, ("Received count of %u octets, data in packet is %ld\n",
				  count, (long)n - CTL_HEADER_LEN));
			continue;
		}
		if (count == 0 && CTL_ISMORE(rpkt.r_m_e_op)) {
			TRACE(1, ("Received count of 0 in non-final fragment\n"));
			continue;
		}
		if (offset + count > sizeof(pktdata)) {
			TRACE(1, ("Offset %u, count %u, too big for buffer\n",
				  offset, count));
			return ERR_TOOMUCH;
		}
		if (seenlastfrag && !CTL_ISMORE(rpkt.r_m_e_op)) {
			TRACE(1, ("Received second last fragment packet\n"));
			continue;
		}

		/*
		 * So far, so good.  Record this fragment, making sure it doesn't
		 * overlap anything.
		 */
		TRACE(2, ("Packet okay\n"));

		if (numfrags > (MAXFRAGS - 1)) {
			TRACE(2, ("Number of fragments exceeds maximum %d\n",
				  MAXFRAGS - 1));
			return ERR_TOOMUCH;
		}

		/*
		 * Find the position for the fragment relative to any
		 * previously received.
		 */
		for (f = 0;
		     f < numfrags && offsets[f] < offset;
		     f++) {
			/* empty body */ ;
		}

		if (f < numfrags && offset == offsets[f]) {
			TRACE(1, ("duplicate %u octets at %u ignored, prior %u at %u\n",
				  count, offset, counts[f], offsets[f]));
			continue;
		}

		if (f > 0 && (offsets[f-1] + counts[f-1]) > offset) {
			TRACE(1, ("received frag at %u overlaps with %u octet frag at %u\n",
				  offset, counts[f-1], offsets[f-1]));
			continue;
		}

		if (f < numfrags && (offset + count) > offsets[f]) {
			TRACE(1, ("received %u octet frag at %u overlaps with frag at %u\n",
				  count, offset, offsets[f]));
			continue;
		}

		for (ff = numfrags; ff > f; ff--) {
			offsets[ff] = offsets[ff-1];
			counts[ff] = counts[ff-1];
		}
		offsets[f] = offset;
		counts[f] = count;
		numfrags++;

		/*
		 * Got that stuffed in right.  Figure out if this was the last.
		 * Record status info out of the last packet.
		 */
		if (!CTL_ISMORE(rpkt.r_m_e_op)) {
			seenlastfrag = 1;
			if (rstatus != 0)
				*rstatus = ntohs(rpkt.status);
		}

		/*
		 * Copy the data into the data buffer.
		 */
		memcpy((char *)pktdata + offset, &rpkt.u, count);

		/*
		 * If we've seen the last fragment, look for holes in the sequence.
		 * If there aren't any, we're done.
		 */
		if (seenlastfrag && offsets[0] == 0) {
			for (f = 1; f < numfrags; f++)
				if (offsets[f-1] + counts[f-1] !=
				    offsets[f])
					break;
			if (f == numfrags) {
				*rsize = offsets[f-1] + counts[f-1];
				TRACE(1, ("%lu packets reassembled into response\n",
					  (u_long)numfrags));
				return 0;
			}
		}
	}  /* giant for (;;) collecting response packets */
}  /* getresponse() */