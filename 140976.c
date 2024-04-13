GetTypeBufSize(matvar_t *matvar, size_t *size)
{
    int err;
    size_t nBytes, data_bytes;
    size_t tag_size = 8;
    size_t nelems = 1;
    size_t rank_size;

    *size = 0;

    err = Mat_MulDims(matvar, &nelems);
    if ( err )
        return err;

    /* Add rank and dimensions, padded to an 8 byte block */
    err = Mul(&rank_size, matvar->rank, 4);
    if ( err )
        return err;

    if ( matvar->rank % 2 )
        nBytes = tag_size + 4;
    else
        nBytes = tag_size;

    err = Add(&nBytes, nBytes, rank_size);
    if ( err )
        return err;

    switch ( matvar->class_type ) {
        case MAT_C_STRUCT: {
            matvar_t **fields = (matvar_t **)matvar->data;
            size_t nfields = matvar->internal->num_fields;
            size_t maxlen = 0, i, field_buf_size;

            for ( i = 0; i < nfields; i++ ) {
                char *fieldname = matvar->internal->fieldnames[i];
                if ( NULL != fieldname && strlen(fieldname) > maxlen )
                    maxlen = strlen(fieldname);
            }
            maxlen++;
            while ( nfields * maxlen % 8 != 0 )
                maxlen++;

            err = Mul(&field_buf_size, maxlen, nfields);
            if ( err )
                return err;
            err = Add(&nBytes, nBytes, tag_size + tag_size);
            if ( err )
                return err;
            err = Add(&nBytes, nBytes, field_buf_size);
            if ( err )
                return err;

            /* FIXME: Add bytes for the fieldnames */
            if ( NULL != fields && nfields > 0 ) {
                size_t nelems_x_nfields = 1;
                err = Mul(&nelems_x_nfields, nelems, nfields);
                if ( err )
                    return err;

                for ( i = 0; i < nelems_x_nfields; i++ ) {
                    err = GetStructFieldBufSize(fields[i], &field_buf_size);
                    if ( err )
                        return err;
                    err = Add(&nBytes, nBytes, tag_size);
                    if ( err )
                        return err;
                    err = Add(&nBytes, nBytes, field_buf_size);
                    if ( err )
                        return err;
                }
            }
            break;
        }
        case MAT_C_CELL: {
            matvar_t **cells = (matvar_t **)matvar->data;

            if ( matvar->nbytes == 0 || matvar->data_size == 0 )
                break;

            nelems = matvar->nbytes / matvar->data_size;
            if ( NULL != cells && nelems > 0 ) {
                size_t i, field_buf_size;
                for ( i = 0; i < nelems; i++ ) {
                    err = GetCellArrayFieldBufSize(cells[i], &field_buf_size);
                    if ( err )
                        return err;
                    err = Add(&nBytes, nBytes, tag_size);
                    if ( err )
                        return err;
                    err = Add(&nBytes, nBytes, field_buf_size);
                    if ( err )
                        return err;
                }
            }
            break;
        }
        case MAT_C_SPARSE: {
            mat_sparse_t *sparse = (mat_sparse_t *)matvar->data;

            err = Mul(&data_bytes, sparse->nir, sizeof(mat_uint32_t));
            if ( err )
                return err;
            if ( data_bytes % 8 ) {
                err = Add(&data_bytes, data_bytes, 8 - data_bytes % 8);
                if ( err )
                    return err;
            }
            err = Add(&nBytes, nBytes, tag_size);
            if ( err )
                return err;
            err = Add(&nBytes, nBytes, data_bytes);
            if ( err )
                return err;

            err = Mul(&data_bytes, sparse->njc, sizeof(mat_uint32_t));
            if ( err )
                return err;
            if ( data_bytes % 8 ) {
                err = Add(&data_bytes, data_bytes, 8 - data_bytes % 8);
                if ( err )
                    return err;
            }
            err = Add(&nBytes, nBytes, tag_size);
            if ( err )
                return err;
            err = Add(&nBytes, nBytes, data_bytes);
            if ( err )
                return err;

            err = Mul(&data_bytes, sparse->ndata, Mat_SizeOf(matvar->data_type));
            if ( err )
                return err;
            if ( data_bytes % 8 ) {
                err = Add(&data_bytes, data_bytes, 8 - data_bytes % 8);
                if ( err )
                    return err;
            }
            err = Add(&nBytes, nBytes, tag_size);
            if ( err )
                return err;
            err = Add(&nBytes, nBytes, data_bytes);
            if ( err )
                return err;

            if ( matvar->isComplex ) {
                err = Add(&nBytes, nBytes, tag_size);
                if ( err )
                    return err;
                err = Add(&nBytes, nBytes, data_bytes);
                if ( err )
                    return err;
            }

            break;
        }
        case MAT_C_CHAR:
            if ( MAT_T_UINT8 == matvar->data_type || MAT_T_INT8 == matvar->data_type )
                err = Mul(&data_bytes, nelems, Mat_SizeOf(MAT_T_UINT16));
            else
                err = Mul(&data_bytes, nelems, Mat_SizeOf(matvar->data_type));
            if ( err )
                return err;
            if ( data_bytes % 8 ) {
                err = Add(&data_bytes, data_bytes, 8 - data_bytes % 8);
                if ( err )
                    return err;
            }

            err = Add(&nBytes, nBytes, tag_size);
            if ( err )
                return err;
            err = Add(&nBytes, nBytes, data_bytes);
            if ( err )
                return err;

            if ( matvar->isComplex ) {
                err = Add(&nBytes, nBytes, tag_size);
                if ( err )
                    return err;
                err = Add(&nBytes, nBytes, data_bytes);
                if ( err )
                    return err;
            }

            break;
        default:
            err = Mul(&data_bytes, nelems, Mat_SizeOf(matvar->data_type));
            if ( err )
                return err;
            if ( data_bytes % 8 ) {
                err = Add(&data_bytes, data_bytes, 8 - data_bytes % 8);
                if ( err )
                    return err;
            }

            err = Add(&nBytes, nBytes, tag_size);
            if ( err )
                return err;
            err = Add(&nBytes, nBytes, data_bytes);
            if ( err )
                return err;

            if ( matvar->isComplex ) {
                err = Add(&nBytes, nBytes, tag_size);
                if ( err )
                    return err;
                err = Add(&nBytes, nBytes, data_bytes);
                if ( err )
                    return err;
            }
    } /* switch ( matvar->class_type ) */

    *size = nBytes;
    return MATIO_E_NO_ERROR;
}