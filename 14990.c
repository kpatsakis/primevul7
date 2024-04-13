generalizedTimeValidate(
	Syntax *syntax,
	struct berval *in )
{
	int parts[9];
	struct berval fraction;
	return check_time_syntax(in, 0, parts, &fraction);
}