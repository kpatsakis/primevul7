kill_labels(TERMTYPE2 *tterm, int target)
{
    int n;
    int result = 0;
    char *cap;
    char name[10];

    for (n = 0; n <= 10; ++n) {
	_nc_SPRINTF(name, _nc_SLIMIT(sizeof(name)) "lf%d", n);
	cap = find_string(tterm, name);
	if (VALID_STRING(cap)
	    && kill_string(tterm, cap)) {
	    target -= (int) (strlen(cap) + 5);
	    ++result;
	    if (target < 0)
		break;
	}
    }
    return result;
}