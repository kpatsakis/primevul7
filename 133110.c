BOOL license_send(rdpLicense* license, wStream* s, BYTE type)
{
	int length;
	BYTE flags;
	UINT16 wMsgSize;
	UINT16 sec_flags;

	DEBUG_LICENSE("Sending %s Packet", LICENSE_MESSAGE_STRINGS[type & 0x1F]);

	length = Stream_GetPosition(s);
	Stream_SetPosition(s, 0);

	sec_flags = SEC_LICENSE_PKT;
	wMsgSize = length - LICENSE_PACKET_HEADER_MAX_LENGTH + 4;

	flags = PREAMBLE_VERSION_3_0;

	/**
	 * Using EXTENDED_ERROR_MSG_SUPPORTED here would cause mstsc to crash when
	 * running in server mode! This flag seems to be incorrectly documented.
	 */

	if (!license->rdp->settings->ServerMode)
		flags |= EXTENDED_ERROR_MSG_SUPPORTED;

	rdp_write_header(license->rdp, s, length, MCS_GLOBAL_CHANNEL_ID);
	rdp_write_security_header(s, sec_flags);
	license_write_preamble(s, type, flags, wMsgSize);

#ifdef WITH_DEBUG_LICENSE
	fprintf(stderr, "Sending %s Packet, length %d\n", LICENSE_MESSAGE_STRINGS[type & 0x1F], wMsgSize);
	winpr_HexDump(Stream_Pointer(s) - 4, wMsgSize);
#endif

	Stream_SetPosition(s, length);
	Stream_SealLength(s);

	if (transport_write(license->rdp->transport, s) < 0)
		return FALSE;

	Stream_Free(s, TRUE);

	return TRUE;
}