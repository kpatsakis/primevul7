static inline int is_utf8_continuation(unsigned char c)
{
	return (c & 0xc0) == 0x80;
}