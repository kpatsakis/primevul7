wStream* license_send_stream_init(rdpLicense* license)
{
	wStream* s;

	s = Stream_New(NULL, 4096);
	Stream_Seek(s, LICENSE_PACKET_HEADER_MAX_LENGTH);

	return s;
}