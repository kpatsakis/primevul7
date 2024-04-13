re_num_cmp(long_u val, char_u *scan)
{
    long_u  n = OPERAND_MIN(scan);

    if (OPERAND_CMP(scan) == '>')
	return val > n;
    if (OPERAND_CMP(scan) == '<')
	return val < n;
    return val == n;
}