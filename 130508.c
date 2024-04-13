number_format(int value)
{
    const char *result = "%d";
    if ((outform != F_TERMCAP) && (value > 255)) {
	unsigned long lv = (unsigned long) value;
	unsigned long mm;
	int bits = sizeof(unsigned long) * 8;
	int nn;
	for (nn = 8; nn < bits; ++nn) {
	    mm = 1UL << nn;
	    if ((mm - 16) <= lv && (mm + 16) > lv) {
		result = "%#x";
		break;
	    }
	}
    }
    return result;
}