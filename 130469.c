find_split(const char *src, int step, int size)
{
    int result = size;
    int n;
    if (size > 0) {
	/* check if that would split a backslash-sequence */
	int mark = size;
	for (n = size - 1; n > 0; --n) {
	    int ch = UChar(src[step + n]);
	    if (ch == '\\') {
		if (n > 0 && src[step + n - 1] == ch)
		    --n;
		mark = n;
		break;
	    } else if (!isalnum(ch)) {
		break;
	    }
	}
	if (mark < size) {
	    result = mark;
	} else {
	    /* check if that would split a backslash-sequence */
	    for (n = size - 1; n > 0; --n) {
		int ch = UChar(src[step + n]);
		if (ch == '%') {
		    int need = op_length(src, step + n);
		    if ((n + need) > size) {
			mark = n;
		    }
		    break;
		}
	    }
	    if (mark < size) {
		result = mark;
	    }
	}
    }
    return result;
}