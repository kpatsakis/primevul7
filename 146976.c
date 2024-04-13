xz_open(const char *path, int fd, const char *mode ATTRIBUTE_UNUSED)
{
    xz_statep state;

    /* allocate xzFile structure to return */
    state = xmlMalloc(sizeof(xz_state));
    if (state == NULL)
        return NULL;
    state->size = 0;            /* no buffers allocated yet */
    state->want = BUFSIZ;       /* requested buffer size */
    state->msg = NULL;          /* no error message yet */
    state->init = 0;            /* initialization of zlib data */

    /* save the path name for error messages */
    state->path = xmlMalloc(strlen(path) + 1);
    if (state->path == NULL) {
        xmlFree(state);
        return NULL;
    }
    strcpy(state->path, path);

    /* open the file with the appropriate mode (or just use fd) */
    state->fd = fd != -1 ? fd : open(path,
#ifdef O_LARGEFILE
                                     O_LARGEFILE |
#endif
#ifdef O_BINARY
                                     O_BINARY |
#endif
                                     O_RDONLY, 0666);
    if (state->fd == -1) {
        xmlFree(state->path);
        xmlFree(state);
        return NULL;
    }

    /* save the current position for rewinding (only if reading) */
    state->start = lseek(state->fd, 0, SEEK_CUR);
    if (state->start == (uint64_t) - 1)
        state->start = 0;

    /* initialize stream */
    xz_reset(state);

    /* return stream */
    return (xzFile) state;
}