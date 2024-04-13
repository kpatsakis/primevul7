op_length(const char *src, int offset)
{
    int result = 0;
    int ch;
    if (offset > 0 && src[offset - 1] == '\\') {
	result = 0;
    } else {
	result++;		/* for '%' mark */
	ch = src[offset + result];
	if (TcOutput()) {
	    if (ch == '>') {
		result += 3;
	    } else if (ch == '+') {
		result += 2;
	    } else {
		result++;
	    }
	} else if (ch == '\'') {
	    result += 3;
	} else if (ch == L_CURL[0]) {
	    int n = result;
	    while ((ch = src[offset + n]) != '\0') {
		if (ch == R_CURL[0]) {
		    result = ++n;
		    break;
		}
		n++;
	    }
	} else if (strchr("pPg", ch) != 0) {
	    result += 2;
	} else {
	    result++;		/* ordinary operator */
	}
    }
    return result;
}