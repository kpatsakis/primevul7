__libxml2_xzclose(xzFile file)
{
    int ret;
    xz_statep state;

    /* get internal structure */
    if (file == NULL)
        return LZMA_DATA_ERROR;
    state = (xz_statep) file;

    /* free memory and close file */
    if (state->size) {
        lzma_end(&(state->strm));
#ifdef LIBXML_ZLIB_ENABLED
        if (state->init == 1)
            inflateEnd(&(state->zstrm));
        state->init = 0;
#endif
        xmlFree(state->out);
        xmlFree(state->in);
    }
    xmlFree(state->path);
    if ((state->msg != NULL) && (state->err != LZMA_MEM_ERROR))
        xmlFree(state->msg);
    ret = close(state->fd);
    xmlFree(state);
    return ret ? ret : LZMA_OK;
}