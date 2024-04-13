xz_compressed(xzFile f) {
    xz_statep state;

    if (f == NULL)
        return(-1);
    state = (xz_statep) f;
    if (state->init <= 0)
        return(-1);

    switch (state->how) {
        case COPY:
	    return(0);
	case GZIP:
	case LZMA:
	    return(1);
    }
    return(-1);
}