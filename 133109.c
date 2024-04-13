int license_recv(rdpLicense* license, wStream* s)
{
	BYTE flags;
	BYTE bMsgType;
	UINT16 wMsgSize;
	UINT16 length;
	UINT16 channelId;
	UINT16 securityFlags;

	if (!rdp_read_header(license->rdp, s, &length, &channelId))
	{
		fprintf(stderr, "Incorrect RDP header.\n");
		return -1;
	}

	if (!rdp_read_security_header(s, &securityFlags))
		return -1;

	if (securityFlags & SEC_ENCRYPT)
	{
		if (!rdp_decrypt(license->rdp, s, length - 4, securityFlags))
		{
			fprintf(stderr, "rdp_decrypt failed\n");
			return -1;
		}
	}

	if (!(securityFlags & SEC_LICENSE_PKT))
	{
		int status;

		if (!(securityFlags & SEC_ENCRYPT))
			Stream_Rewind(s, RDP_SECURITY_HEADER_LENGTH);

		status = rdp_recv_out_of_sequence_pdu(license->rdp, s);

		if (status < 0)
		{
			fprintf(stderr, "Unexpected license packet.\n");
			return status;
		}

		return 0;
	}

	if (!license_read_preamble(s, &bMsgType, &flags, &wMsgSize)) /* preamble (4 bytes) */
		return -1;

	DEBUG_LICENSE("Receiving %s Packet", LICENSE_MESSAGE_STRINGS[bMsgType & 0x1F]);

	switch (bMsgType)
	{
		case LICENSE_REQUEST:
			if (!license_read_license_request_packet(license, s))
				return -1;
			license_send_new_license_request_packet(license);
			break;

		case PLATFORM_CHALLENGE:
			if (!license_read_platform_challenge_packet(license, s))
				return -1;
			license_send_platform_challenge_response_packet(license);
			break;

		case NEW_LICENSE:
			license_read_new_license_packet(license, s);
			break;

		case UPGRADE_LICENSE:
			license_read_upgrade_license_packet(license, s);
			break;

		case ERROR_ALERT:
			if (!license_read_error_alert_packet(license, s))
				return -1;
			break;

		default:
			fprintf(stderr, "invalid bMsgType:%d\n", bMsgType);
			return FALSE;
	}

	return 0;
}