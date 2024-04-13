void license_send_new_license_request_packet(rdpLicense* license)
{
	wStream* s;
	char* username;

	DEBUG_LICENSE("Sending New License Packet");

	s = license_send_stream_init(license);

	if (license->rdp->settings->Username != NULL)
		username = license->rdp->settings->Username;
	else
		username = "username";

	license->ClientUserName->data = (BYTE*) username;
	license->ClientUserName->length = strlen(username) + 1;

	license->ClientMachineName->data = (BYTE*) license->rdp->settings->ClientHostname;
	license->ClientMachineName->length = strlen(license->rdp->settings->ClientHostname) + 1;

	license_write_new_license_request_packet(license, s);

	license_send(license, s, NEW_LICENSE_REQUEST);

	license->ClientUserName->data = NULL;
	license->ClientUserName->length = 0;

	license->ClientMachineName->data = NULL;
	license->ClientMachineName->length = 0;
}