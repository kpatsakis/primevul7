XFreeFontInfo (
    char **names,
    XFontStruct *info,
    int actualCount)
{
	register int i;
	if (names) {
		Xfree (names[0]-1);
		for (i = 1; i < actualCount; i++) {
			Xfree (names[i]);
		}
		Xfree(names);
	}
	if (info) {
		for (i = 0; i < actualCount; i++) {
			if (info[i].per_char)
#ifdef USE_XF86BIGFONT
				_XF86BigfontFreeFontMetrics(&info[i]);
#else
				Xfree (info[i].per_char);
#endif
			if (info[i].properties)
				Xfree (info[i].properties);
			}
		Xfree(info);
	}
	return 1;
}