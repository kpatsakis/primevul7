static int sanitary_path_copy(char *dst, const char *src)
{
	char *dst0 = dst;

	if (*src == '/') {
		*dst++ = '/';
		while (*src == '/')
			src++;
	}

	for (;;) {
		char c = *src;

		/*
		 * A path component that begins with . could be
		 * special:
		 * (1) "." and ends   -- ignore and terminate.
		 * (2) "./"           -- ignore them, eat slash and continue.
		 * (3) ".." and ends  -- strip one and terminate.
		 * (4) "../"          -- strip one, eat slash and continue.
		 */
		if (c == '.') {
			switch (src[1]) {
			case '\0':
				/* (1) */
				src++;
				break;
			case '/':
				/* (2) */
				src += 2;
				while (*src == '/')
					src++;
				continue;
			case '.':
				switch (src[2]) {
				case '\0':
					/* (3) */
					src += 2;
					goto up_one;
				case '/':
					/* (4) */
					src += 3;
					while (*src == '/')
						src++;
					goto up_one;
				}
			}
		}

		/* copy up to the next '/', and eat all '/' */
		while ((c = *src++) != '\0' && c != '/')
			*dst++ = c;
		if (c == '/') {
			*dst++ = c;
			while (c == '/')
				c = *src++;
			src--;
		} else if (!c)
			break;
		continue;

	up_one:
		/*
		 * dst0..dst is prefix portion, and dst[-1] is '/';
		 * go up one level.
		 */
		dst -= 2; /* go past trailing '/' if any */
		if (dst < dst0)
			return -1;
		while (1) {
			if (dst <= dst0)
				break;
			c = *dst--;
			if (c == '/') {
				dst += 2;
				break;
			}
		}
	}
	*dst = '\0';
	return 0;
}