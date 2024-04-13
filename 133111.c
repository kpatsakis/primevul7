void license_read_upgrade_license_packet(rdpLicense* license, wStream* s)
{
	DEBUG_LICENSE("Receiving Upgrade License Packet");
	license->state = LICENSE_STATE_COMPLETED;
}