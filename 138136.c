/* {{{ date_interval_format -  */
static char *date_interval_format(char *format, int format_len, timelib_rel_time *t)
{
	smart_str            string = {0};
	int                  i, length, have_format_spec = 0;
	char                 buffer[33];

	if (!format_len) {
		return estrdup("");
	}

	for (i = 0; i < format_len; i++) {
		if (have_format_spec) {
			switch (format[i]) {
				case 'Y': length = slprintf(buffer, 32, "%02d", (int) t->y); break;
				case 'y': length = slprintf(buffer, 32, "%d", (int) t->y); break;

				case 'M': length = slprintf(buffer, 32, "%02d", (int) t->m); break;
				case 'm': length = slprintf(buffer, 32, "%d", (int) t->m); break;

				case 'D': length = slprintf(buffer, 32, "%02d", (int) t->d); break;
				case 'd': length = slprintf(buffer, 32, "%d", (int) t->d); break;

				case 'H': length = slprintf(buffer, 32, "%02d", (int) t->h); break;
				case 'h': length = slprintf(buffer, 32, "%d", (int) t->h); break;

				case 'I': length = slprintf(buffer, 32, "%02d", (int) t->i); break;
				case 'i': length = slprintf(buffer, 32, "%d", (int) t->i); break;

				case 'S': length = slprintf(buffer, 32, "%02ld", (long) t->s); break;
				case 's': length = slprintf(buffer, 32, "%ld", (long) t->s); break;

				case 'a': {
					if ((int) t->days != -99999) {
						length = slprintf(buffer, 32, "%d", (int) t->days);
					} else {
						length = slprintf(buffer, 32, "(unknown)");
					}
				} break;
				case 'r': length = slprintf(buffer, 32, "%s", t->invert ? "-" : ""); break;
				case 'R': length = slprintf(buffer, 32, "%c", t->invert ? '-' : '+'); break;

				case '%': length = slprintf(buffer, 32, "%%"); break;
				default: buffer[0] = '%'; buffer[1] = format[i]; buffer[2] = '\0'; length = 2; break;
			}
			smart_str_appendl(&string, buffer, length);
			have_format_spec = 0;
		} else {
			if (format[i] == '%') {
				have_format_spec = 1;
			} else {
				smart_str_appendc(&string, format[i]);
			}
		}
	}

	smart_str_0(&string);

	return string.c;