static void security_UINT32_le(BYTE* output, UINT32 value)
{
	output[0] = (value)&0xFF;
	output[1] = (value >> 8) & 0xFF;
	output[2] = (value >> 16) & 0xFF;
	output[3] = (value >> 24) & 0xFF;
}