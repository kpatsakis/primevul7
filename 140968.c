ReadRankDims(mat_t *mat, matvar_t *matvar, enum matio_types data_type, mat_uint32_t nbytes,
             size_t *read_bytes)
{
    int err = MATIO_E_NO_ERROR;
    /* Rank and dimension */
    if ( data_type == MAT_T_INT32 ) {
        matvar->rank = nbytes / sizeof(mat_uint32_t);
        matvar->dims = (size_t *)malloc(matvar->rank * sizeof(*matvar->dims));
        if ( NULL != matvar->dims ) {
            int i;
            mat_uint32_t buf;

            for ( i = 0; i < matvar->rank; i++ ) {
                err = Read(&buf, sizeof(mat_uint32_t), 1, (FILE *)mat->fp, read_bytes);
                if ( MATIO_E_NO_ERROR == err ) {
                    if ( mat->byteswap ) {
                        matvar->dims[i] = Mat_uint32Swap(&buf);
                    } else {
                        matvar->dims[i] = buf;
                    }
                } else {
                    free(matvar->dims);
                    matvar->dims = NULL;
                    matvar->rank = 0;
                    return err;
                }
            }

            if ( matvar->rank % 2 != 0 ) {
                err = Read(&buf, sizeof(mat_uint32_t), 1, (FILE *)mat->fp, read_bytes);
                if ( err ) {
                    free(matvar->dims);
                    matvar->dims = NULL;
                    matvar->rank = 0;
                    return err;
                }
            }
        } else {
            matvar->rank = 0;
            err = MATIO_E_OUT_OF_MEMORY;
            Mat_Critical("Error allocating memory for dims");
        }
    }
    return err;
}