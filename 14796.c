static int check_crc_flex(unsigned char *cp, int size)
{
	unsigned short crc = 0xffff;

	if (size < 3)
		return -1;

	while (size--)
		crc = (crc << 8) ^ crc_flex_table[((crc >> 8) ^ *cp++) & 0xff];

	if ((crc & 0xffff) != 0x7070)
		return -1;

	return 0;
}