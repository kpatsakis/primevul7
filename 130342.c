static int jas_strtoopenmode(const char *s)
{
	int openmode = 0;
	while (*s != '\0') {
		switch (*s) {
		case 'r':
			openmode |= JAS_STREAM_READ;
			break;
		case 'w':
			openmode |= JAS_STREAM_WRITE | JAS_STREAM_CREATE;
			break;
		case 'b':
			openmode |= JAS_STREAM_BINARY;
			break;
		case 'a':
			openmode |= JAS_STREAM_APPEND;
			break;
		case '+':
			openmode |= JAS_STREAM_READ | JAS_STREAM_WRITE;
			break;
		default:
			break;
		}
		++s;
	}
	return openmode;
}