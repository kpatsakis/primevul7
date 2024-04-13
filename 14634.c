hexchar(const char *s)
{
	unsigned char result[2];
	int i;

	for (i = 0; i < 2; i++) {
		if (s[i] >= '0' && s[i] <= '9')
			result[i] = (unsigned char)(s[i] - '0');
		else if (s[i] >= 'a' && s[i] <= 'f')
			result[i] = (unsigned char)(s[i] - 'a') + 10;
		else if (s[i] >= 'A' && s[i] <= 'F')
			result[i] = (unsigned char)(s[i] - 'A') + 10;
		else
			return -1;
	}
	return (result[0] << 4) | result[1];
}