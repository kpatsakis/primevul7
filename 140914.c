ctl_putcal(
	const char *tag,
	const struct calendar *pcal
	)
{
	char buffer[100];
	unsigned numch;

	numch = snprintf(buffer, sizeof(buffer),
			"%s=%04d%02d%02d%02d%02d",
			tag,
			pcal->year,
			pcal->month,
			pcal->monthday,
			pcal->hour,
			pcal->minute
			);
	INSIST(numch < sizeof(buffer));
	ctl_putdata(buffer, numch, 0);

	return;
}