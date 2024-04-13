void license_read_new_license_packet(rdpLicense* license, wStream* s)
{
	DEBUG_LICENSE("Receiving New License Packet");
	license->state = LICENSE_STATE_COMPLETED;
}