xz_error(xz_statep state, int err, const char *msg)
{
    /* free previously allocated message and clear */
    if (state->msg != NULL) {
        if (state->err != LZMA_MEM_ERROR)
            xmlFree(state->msg);
        state->msg = NULL;
    }

    /* set error code, and if no message, then done */
    state->err = err;
    if (msg == NULL)
        return;

    /* for an out of memory error, save as static string */
    if (err == LZMA_MEM_ERROR) {
        state->msg = (char *) msg;
        return;
    }

    /* construct error message with path */
    if ((state->msg =
         xmlMalloc(strlen(state->path) + strlen(msg) + 3)) == NULL) {
        state->err = LZMA_MEM_ERROR;
        state->msg = (char *) "out of memory";
        return;
    }
    strcpy(state->msg, state->path);
    strcat(state->msg, ": ");
    strcat(state->msg, msg);
    return;
}