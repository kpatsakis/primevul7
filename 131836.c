adjust_sign(int is_negative, int forcesign, int *signvalue)
{
	if (is_negative)
	{
		*signvalue = '-';
		return true;
	}
	else if (forcesign)
		*signvalue = '+';
	return false;
}