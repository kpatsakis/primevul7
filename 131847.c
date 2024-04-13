adjust_padlen(int minlen, int vallen, int leftjust, int *padlen)
{
	*padlen = minlen - vallen;
	if (*padlen < 0)
		*padlen = 0;
	if (leftjust)
		*padlen = -(*padlen);
}