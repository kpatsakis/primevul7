BOOL license_send_valid_client_error_packet(rdpLicense* license)
{
	wStream* s;

	s = license_send_stream_init(license);

	DEBUG_LICENSE("Sending Error Alert Packet");

	Stream_Write_UINT32(s, STATUS_VALID_CLIENT); /* dwErrorCode */
	Stream_Write_UINT32(s, ST_NO_TRANSITION); /* dwStateTransition */

	license_write_binary_blob(s, license->ErrorInfo);

	return license_send(license, s, ERROR_ALERT);
}