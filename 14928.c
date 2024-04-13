utcTimeValidate(
	Syntax *syntax,
	struct berval *in )
{
	int parts[9];
	return check_time_syntax(in, 1, parts, NULL);
}