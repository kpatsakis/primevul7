Mat_VarRead5(mat_t *mat, matvar_t *matvar)
{
    int nBytes = 0, byteswap, data_in_tag = 0, err;
    size_t nelems = 1;
    enum matio_types packed_type = MAT_T_UNKNOWN;
    long fpos;
    mat_uint32_t tag[2] = {0, 0};
    size_t bytesread = 0;

    if ( matvar == NULL )
        return MATIO_E_BAD_ARGUMENT;
    else if ( matvar->rank == 0 ) /* An empty data set */
        return MATIO_E_NO_ERROR;
#if HAVE_ZLIB
    else if ( NULL != matvar->internal->data ) {
        /* Data already read in ReadNextStructField or ReadNextCell */
        matvar->data = matvar->internal->data;
        matvar->internal->data = NULL;
        return MATIO_E_NO_ERROR;
    }
#endif
    fpos = ftell((FILE *)mat->fp);
    if ( fpos == -1L ) {
        Mat_Critical("Couldn't determine file position");
        return MATIO_E_GENERIC_READ_ERROR;
    }
    err = Mat_MulDims(matvar, &nelems);
    if ( err ) {
        Mat_Critical("Integer multiplication overflow");
        return err;
    }
    byteswap = mat->byteswap;
    switch ( matvar->class_type ) {
        case MAT_C_EMPTY:
            matvar->nbytes = 0;
            matvar->data_size = sizeof(double);
            matvar->data_type = MAT_T_DOUBLE;
            matvar->rank = 2;
            if ( NULL != matvar->dims ) {
                free(matvar->dims);
            }
            matvar->dims = (size_t *)calloc(matvar->rank, sizeof(*(matvar->dims)));
            break;
        case MAT_C_DOUBLE:
            (void)fseek((FILE *)mat->fp, matvar->internal->datapos, SEEK_SET);
            matvar->data_size = sizeof(double);
            matvar->data_type = MAT_T_DOUBLE;
            break;
        case MAT_C_SINGLE:
            (void)fseek((FILE *)mat->fp, matvar->internal->datapos, SEEK_SET);
            matvar->data_size = sizeof(float);
            matvar->data_type = MAT_T_SINGLE;
            break;
        case MAT_C_INT64:
#ifdef HAVE_MAT_INT64_T
            (void)fseek((FILE *)mat->fp, matvar->internal->datapos, SEEK_SET);
            matvar->data_size = sizeof(mat_int64_t);
            matvar->data_type = MAT_T_INT64;
#endif
            break;
        case MAT_C_UINT64:
#ifdef HAVE_MAT_UINT64_T
            (void)fseek((FILE *)mat->fp, matvar->internal->datapos, SEEK_SET);
            matvar->data_size = sizeof(mat_uint64_t);
            matvar->data_type = MAT_T_UINT64;
#endif
            break;
        case MAT_C_INT32:
            (void)fseek((FILE *)mat->fp, matvar->internal->datapos, SEEK_SET);
            matvar->data_size = sizeof(mat_int32_t);
            matvar->data_type = MAT_T_INT32;
            break;
        case MAT_C_UINT32:
            (void)fseek((FILE *)mat->fp, matvar->internal->datapos, SEEK_SET);
            matvar->data_size = sizeof(mat_uint32_t);
            matvar->data_type = MAT_T_UINT32;
            break;
        case MAT_C_INT16:
            (void)fseek((FILE *)mat->fp, matvar->internal->datapos, SEEK_SET);
            matvar->data_size = sizeof(mat_int16_t);
            matvar->data_type = MAT_T_INT16;
            break;
        case MAT_C_UINT16:
            (void)fseek((FILE *)mat->fp, matvar->internal->datapos, SEEK_SET);
            matvar->data_size = sizeof(mat_uint16_t);
            matvar->data_type = MAT_T_UINT16;
            break;
        case MAT_C_INT8:
            (void)fseek((FILE *)mat->fp, matvar->internal->datapos, SEEK_SET);
            matvar->data_size = sizeof(mat_int8_t);
            matvar->data_type = MAT_T_INT8;
            break;
        case MAT_C_UINT8:
            (void)fseek((FILE *)mat->fp, matvar->internal->datapos, SEEK_SET);
            matvar->data_size = sizeof(mat_uint8_t);
            matvar->data_type = MAT_T_UINT8;
            break;
        case MAT_C_CHAR:
            (void)fseek((FILE *)mat->fp, matvar->internal->datapos, SEEK_SET);
            if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
#if HAVE_ZLIB
                matvar->internal->z->avail_in = 0;
                err = Inflate(mat, matvar->internal->z, tag, 4, &bytesread);
                if ( err ) {
                    break;
                }
                if ( byteswap )
                    (void)Mat_uint32Swap(tag);
                packed_type = TYPE_FROM_TAG(tag[0]);
                if ( tag[0] & 0xffff0000 ) { /* Data is in the tag */
                    data_in_tag = 1;
                    nBytes = (tag[0] & 0xffff0000) >> 16;
                } else {
                    data_in_tag = 0;
                    err = Inflate(mat, matvar->internal->z, tag + 1, 4, &bytesread);
                    if ( err ) {
                        break;
                    }
                    if ( byteswap )
                        (void)Mat_uint32Swap(tag + 1);
                    nBytes = tag[1];
                }
#endif
                matvar->data_type = packed_type;
                matvar->data_size = Mat_SizeOf(matvar->data_type);
                matvar->nbytes = nBytes;
            } else {
                err = Read(tag, 4, 1, (FILE *)mat->fp, &bytesread);
                if ( err ) {
                    break;
                }
                if ( byteswap )
                    (void)Mat_uint32Swap(tag);
                packed_type = TYPE_FROM_TAG(tag[0]);
                if ( tag[0] & 0xffff0000 ) { /* Data is in the tag */
                    data_in_tag = 1;
                    nBytes = (tag[0] & 0xffff0000) >> 16;
                } else {
                    data_in_tag = 0;
                    err = Read(tag + 1, 4, 1, (FILE *)mat->fp, &bytesread);
                    if ( err ) {
                        break;
                    }
                    if ( byteswap )
                        (void)Mat_uint32Swap(tag + 1);
                    nBytes = tag[1];
                }
                matvar->data_type = packed_type;
                matvar->data_size = Mat_SizeOf(matvar->data_type);
                matvar->nbytes = nBytes;
            }
            if ( matvar->isComplex ) {
                break;
            }
            if ( 0 == matvar->nbytes ) {
                matvar->data = calloc(1, 1);
            } else {
                matvar->data = calloc(matvar->nbytes, 1);
            }
            if ( NULL == matvar->data ) {
                err = MATIO_E_OUT_OF_MEMORY;
                Mat_Critical("Couldn't allocate memory for the data");
                break;
            }
            if ( 0 == matvar->nbytes ) {
                break;
            }
            {
                size_t nbytes = 0;
                err = Mul(&nbytes, nelems, matvar->data_size);
                if ( err || nbytes > matvar->nbytes ) {
                    break;
                }
            }
            if ( matvar->data_type == MAT_T_UTF8 ) {
                nelems = matvar->nbytes;
            }
            if ( matvar->compression == MAT_COMPRESSION_NONE ) {
                nBytes = ReadCharData(mat, matvar->data, matvar->data_type, nelems);
                /*
                 * If the data was in the tag we started on a 4-byte
                 * boundary so add 4 to make it an 8-byte
                 */
                if ( data_in_tag )
                    nBytes += 4;
                if ( (nBytes % 8) != 0 )
                    (void)fseek((FILE *)mat->fp, 8 - (nBytes % 8), SEEK_CUR);
#if HAVE_ZLIB
            } else if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
                nBytes = ReadCompressedCharData(mat, matvar->internal->z, matvar->data,
                                                matvar->data_type, nelems);
                /*
                 * If the data was in the tag we started on a 4-byte
                 * boundary so add 4 to make it an 8-byte
                 */
                if ( data_in_tag )
                    nBytes += 4;
                if ( (nBytes % 8) != 0 )
                    InflateSkip(mat, matvar->internal->z, 8 - (nBytes % 8), NULL);
#endif
            }
            break;
        case MAT_C_STRUCT: {
            matvar_t **fields;
            size_t i, nelems_x_nfields;

            matvar->data_type = MAT_T_STRUCT;
            err = Mul(&nelems_x_nfields, nelems, matvar->internal->num_fields);
            if ( err || !matvar->nbytes || !matvar->data_size || NULL == matvar->data )
                break;
            fields = (matvar_t **)matvar->data;
            for ( i = 0; i < nelems_x_nfields; i++ ) {
                if ( NULL != fields[i] ) {
                    err = Mat_VarRead5(mat, fields[i]);
                    if ( err )
                        break;
                }
            }
            break;
        }
        case MAT_C_CELL: {
            matvar_t **cells;
            size_t i;

            if ( NULL == matvar->data ) {
                Mat_Critical("Data is NULL for cell array %s", matvar->name);
                err = MATIO_E_FILE_FORMAT_VIOLATION;
                break;
            }
            cells = (matvar_t **)matvar->data;
            for ( i = 0; i < nelems; i++ ) {
                if ( NULL != cells[i] ) {
                    err = Mat_VarRead5(mat, cells[i]);
                    if ( err )
                        break;
                }
            }
            /* FIXME: */
            matvar->data_type = MAT_T_CELL;
            break;
        }
        case MAT_C_SPARSE: {
            mat_uint32_t N = 0;
            mat_sparse_t *sparse;

            matvar->data_size = sizeof(mat_sparse_t);
            matvar->data = calloc(1, matvar->data_size);
            if ( matvar->data == NULL ) {
                err = MATIO_E_OUT_OF_MEMORY;
                Mat_Critical("Mat_VarRead5: Allocation of data pointer failed");
                break;
            }
            sparse = (mat_sparse_t *)matvar->data;
            sparse->nzmax = matvar->nbytes;
            (void)fseek((FILE *)mat->fp, matvar->internal->datapos, SEEK_SET);
            /*  Read ir    */
            bytesread += ReadSparse(mat, matvar, &sparse->nir, &sparse->ir);
            /*  Read jc    */
            bytesread += ReadSparse(mat, matvar, &sparse->njc, &sparse->jc);
            /*  Read data  */
            if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
#if HAVE_ZLIB
                matvar->internal->z->avail_in = 0;
                err = Inflate(mat, matvar->internal->z, tag, 4, &bytesread);
                if ( err ) {
                    break;
                }
                if ( mat->byteswap )
                    (void)Mat_uint32Swap(tag);
                packed_type = TYPE_FROM_TAG(tag[0]);
                if ( tag[0] & 0xffff0000 ) { /* Data is in the tag */
                    data_in_tag = 1;
                    N = (tag[0] & 0xffff0000) >> 16;
                } else {
                    data_in_tag = 0;
                    (void)ReadCompressedUInt32Data(mat, matvar->internal->z, &N, MAT_T_UINT32, 1);
                }
#endif
            } else {
                err = Read(tag, 4, 1, (FILE *)mat->fp, &bytesread);
                if ( err ) {
                    break;
                }
                if ( mat->byteswap )
                    (void)Mat_uint32Swap(tag);
                packed_type = TYPE_FROM_TAG(tag[0]);
                if ( tag[0] & 0xffff0000 ) { /* Data is in the tag */
                    data_in_tag = 1;
                    N = (tag[0] & 0xffff0000) >> 16;
                } else {
                    data_in_tag = 0;
                    err = Read(&N, 4, 1, (FILE *)mat->fp, &bytesread);
                    if ( err ) {
                        break;
                    }
                    if ( mat->byteswap )
                        (void)Mat_uint32Swap(&N);
                }
            }
            if ( matvar->isLogical && packed_type == MAT_T_DOUBLE ) {
                /* For some reason, MAT says the data type is a double,
                 * but it appears to be written as 8-bit unsigned integer.
                 */
                packed_type = MAT_T_UINT8;
            }
#if defined(EXTENDED_SPARSE)
            matvar->data_type = packed_type;
#else
            matvar->data_type = MAT_T_DOUBLE;
#endif
            {
                size_t s_type = Mat_SizeOf(packed_type);
                if ( s_type == 0 )
                    break;
                sparse->ndata = N / s_type;
            }
            if ( matvar->isComplex ) {
                mat_complex_split_t *complex_data;
                size_t nbytes = 0;
                err = Mul(&nbytes, sparse->ndata, Mat_SizeOf(matvar->data_type));
                if ( err ) {
                    Mat_Critical("Integer multiplication overflow");
                    break;
                }
                complex_data = ComplexMalloc(nbytes);
                if ( NULL == complex_data ) {
                    err = MATIO_E_OUT_OF_MEMORY;
                    Mat_Critical("Couldn't allocate memory for the complex sparse data");
                    break;
                }
                if ( matvar->compression == MAT_COMPRESSION_NONE ) {
#if defined(EXTENDED_SPARSE)
                    switch ( matvar->data_type ) {
                        case MAT_T_DOUBLE:
                            nBytes = ReadDoubleData(mat, (double *)complex_data->Re, packed_type,
                                                    sparse->ndata);
                            break;
                        case MAT_T_SINGLE:
                            nBytes = ReadSingleData(mat, (float *)complex_data->Re, packed_type,
                                                    sparse->ndata);
                            break;
                        case MAT_T_INT64:
#ifdef HAVE_MAT_INT64_T
                            nBytes = ReadInt64Data(mat, (mat_int64_t *)complex_data->Re,
                                                   packed_type, sparse->ndata);
#endif
                            break;
                        case MAT_T_UINT64:
#ifdef HAVE_MAT_UINT64_T
                            nBytes = ReadUInt64Data(mat, (mat_uint64_t *)complex_data->Re,
                                                    packed_type, sparse->ndata);
#endif
                            break;
                        case MAT_T_INT32:
                            nBytes = ReadInt32Data(mat, (mat_int32_t *)complex_data->Re,
                                                   packed_type, sparse->ndata);
                            break;
                        case MAT_T_UINT32:
                            nBytes = ReadUInt32Data(mat, (mat_uint32_t *)complex_data->Re,
                                                    packed_type, sparse->ndata);
                            break;
                        case MAT_T_INT16:
                            nBytes = ReadInt16Data(mat, (mat_int16_t *)complex_data->Re,
                                                   packed_type, sparse->ndata);
                            break;
                        case MAT_T_UINT16:
                            nBytes = ReadUInt16Data(mat, (mat_uint16_t *)complex_data->Re,
                                                    packed_type, sparse->ndata);
                            break;
                        case MAT_T_INT8:
                            nBytes = ReadInt8Data(mat, (mat_int8_t *)complex_data->Re, packed_type,
                                                  sparse->ndata);
                            break;
                        case MAT_T_UINT8:
                            nBytes = ReadUInt8Data(mat, (mat_uint8_t *)complex_data->Re,
                                                   packed_type, sparse->ndata);
                            break;
                        default:
                            break;
                    }
#else
                    nBytes =
                        ReadDoubleData(mat, (double *)complex_data->Re, packed_type, sparse->ndata);
#endif
                    nBytes *= Mat_SizeOf(packed_type);
                    if ( data_in_tag )
                        nBytes += 4;
                    if ( (nBytes % 8) != 0 )
                        (void)fseek((FILE *)mat->fp, 8 - (nBytes % 8), SEEK_CUR);

                    /* Complex Data Tag */
                    err = Read(tag, 4, 1, (FILE *)mat->fp, &bytesread);
                    if ( err ) {
                        ComplexFree(complex_data);
                        break;
                    }
                    if ( byteswap )
                        (void)Mat_uint32Swap(tag);
                    packed_type = TYPE_FROM_TAG(tag[0]);
                    if ( tag[0] & 0xffff0000 ) { /* Data is in the tag */
                        data_in_tag = 1;
                        nBytes = (tag[0] & 0xffff0000) >> 16;
                    } else {
                        data_in_tag = 0;
                        err = Read(tag + 1, 4, 1, (FILE *)mat->fp, &bytesread);
                        if ( err ) {
                            ComplexFree(complex_data);
                            break;
                        }
                        if ( byteswap )
                            (void)Mat_uint32Swap(tag + 1);
                        nBytes = tag[1];
                    }
#if defined(EXTENDED_SPARSE)
                    switch ( matvar->data_type ) {
                        case MAT_T_DOUBLE:
                            nBytes = ReadDoubleData(mat, (double *)complex_data->Im, packed_type,
                                                    sparse->ndata);
                            break;
                        case MAT_T_SINGLE:
                            nBytes = ReadSingleData(mat, (float *)complex_data->Im, packed_type,
                                                    sparse->ndata);
                            break;
                        case MAT_T_INT64:
#ifdef HAVE_MAT_INT64_T
                            nBytes = ReadInt64Data(mat, (mat_int64_t *)complex_data->Im,
                                                   packed_type, sparse->ndata);
#endif
                            break;
                        case MAT_T_UINT64:
#ifdef HAVE_MAT_UINT64_T
                            nBytes = ReadUInt64Data(mat, (mat_uint64_t *)complex_data->Im,
                                                    packed_type, sparse->ndata);
#endif
                            break;
                        case MAT_T_INT32:
                            nBytes = ReadInt32Data(mat, (mat_int32_t *)complex_data->Im,
                                                   packed_type, sparse->ndata);
                            break;
                        case MAT_T_UINT32:
                            nBytes = ReadUInt32Data(mat, (mat_uint32_t *)complex_data->Im,
                                                    packed_type, sparse->ndata);
                            break;
                        case MAT_T_INT16:
                            nBytes = ReadInt16Data(mat, (mat_int16_t *)complex_data->Im,
                                                   packed_type, sparse->ndata);
                            break;
                        case MAT_T_UINT16:
                            nBytes = ReadUInt16Data(mat, (mat_uint16_t *)complex_data->Im,
                                                    packed_type, sparse->ndata);
                            break;
                        case MAT_T_INT8:
                            nBytes = ReadInt8Data(mat, (mat_int8_t *)complex_data->Im, packed_type,
                                                  sparse->ndata);
                            break;
                        case MAT_T_UINT8:
                            nBytes = ReadUInt8Data(mat, (mat_uint8_t *)complex_data->Im,
                                                   packed_type, sparse->ndata);
                            break;
                        default:
                            break;
                    }
#else  /* EXTENDED_SPARSE */
                    nBytes =
                        ReadDoubleData(mat, (double *)complex_data->Im, packed_type, sparse->ndata);
#endif /* EXTENDED_SPARSE */
                    nBytes *= Mat_SizeOf(packed_type);
                    if ( data_in_tag )
                        nBytes += 4;
                    if ( (nBytes % 8) != 0 )
                        (void)fseek((FILE *)mat->fp, 8 - (nBytes % 8), SEEK_CUR);
#if HAVE_ZLIB
                } else if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
#if defined(EXTENDED_SPARSE)
                    switch ( matvar->data_type ) {
                        case MAT_T_DOUBLE:
                            nBytes = ReadCompressedDoubleData(mat, matvar->internal->z,
                                                              (double *)complex_data->Re,
                                                              packed_type, sparse->ndata);
                            break;
                        case MAT_T_SINGLE:
                            nBytes = ReadCompressedSingleData(mat, matvar->internal->z,
                                                              (float *)complex_data->Re,
                                                              packed_type, sparse->ndata);
                            break;
                        case MAT_T_INT64:
#ifdef HAVE_MAT_INT64_T
                            nBytes = ReadCompressedInt64Data(mat, matvar->internal->z,
                                                             (mat_int64_t *)complex_data->Re,
                                                             packed_type, sparse->ndata);
#endif
                            break;
                        case MAT_T_UINT64:
#ifdef HAVE_MAT_UINT64_T
                            nBytes = ReadCompressedUInt64Data(mat, matvar->internal->z,
                                                              (mat_uint64_t *)complex_data->Re,
                                                              packed_type, sparse->ndata);
#endif
                            break;
                        case MAT_T_INT32:
                            nBytes = ReadCompressedInt32Data(mat, matvar->internal->z,
                                                             (mat_int32_t *)complex_data->Re,
                                                             packed_type, sparse->ndata);
                            break;
                        case MAT_T_UINT32:
                            nBytes = ReadCompressedUInt32Data(mat, matvar->internal->z,
                                                              (mat_uint32_t *)complex_data->Re,
                                                              packed_type, sparse->ndata);
                            break;
                        case MAT_T_INT16:
                            nBytes = ReadCompressedInt16Data(mat, matvar->internal->z,
                                                             (mat_int16_t *)complex_data->Re,
                                                             packed_type, sparse->ndata);
                            break;
                        case MAT_T_UINT16:
                            nBytes = ReadCompressedUInt16Data(mat, matvar->internal->z,
                                                              (mat_uint16_t *)complex_data->Re,
                                                              packed_type, sparse->ndata);
                            break;
                        case MAT_T_INT8:
                            nBytes = ReadCompressedInt8Data(mat, matvar->internal->z,
                                                            (mat_int8_t *)complex_data->Re,
                                                            packed_type, sparse->ndata);
                            break;
                        case MAT_T_UINT8:
                            nBytes = ReadCompressedUInt8Data(mat, matvar->internal->z,
                                                             (mat_uint8_t *)complex_data->Re,
                                                             packed_type, sparse->ndata);
                            break;
                        default:
                            break;
                    }
#else  /* EXTENDED_SPARSE */
                    nBytes = ReadCompressedDoubleData(mat, matvar->internal->z,
                                                      (double *)complex_data->Re, packed_type,
                                                      sparse->ndata);
#endif /* EXTENDED_SPARSE */
                    if ( data_in_tag )
                        nBytes += 4;
                    if ( (nBytes % 8) != 0 )
                        InflateSkip(mat, matvar->internal->z, 8 - (nBytes % 8), NULL);

                    /* Complex Data Tag */
                    err = Inflate(mat, matvar->internal->z, tag, 4, NULL);
                    if ( err ) {
                        ComplexFree(complex_data);
                        break;
                    }
                    if ( byteswap )
                        (void)Mat_uint32Swap(tag);

                    packed_type = TYPE_FROM_TAG(tag[0]);
                    if ( tag[0] & 0xffff0000 ) { /* Data is in the tag */
                        data_in_tag = 1;
                        nBytes = (tag[0] & 0xffff0000) >> 16;
                    } else {
                        data_in_tag = 0;
                        err = Inflate(mat, matvar->internal->z, tag + 1, 4, NULL);
                        if ( err ) {
                            ComplexFree(complex_data);
                            break;
                        }
                        if ( byteswap )
                            (void)Mat_uint32Swap(tag + 1);
                        nBytes = tag[1];
                    }
#if defined(EXTENDED_SPARSE)
                    switch ( matvar->data_type ) {
                        case MAT_T_DOUBLE:
                            nBytes = ReadCompressedDoubleData(mat, matvar->internal->z,
                                                              (double *)complex_data->Im,
                                                              packed_type, sparse->ndata);
                            break;
                        case MAT_T_SINGLE:
                            nBytes = ReadCompressedSingleData(mat, matvar->internal->z,
                                                              (float *)complex_data->Im,
                                                              packed_type, sparse->ndata);
                            break;
                        case MAT_T_INT64:
#ifdef HAVE_MAT_INT64_T
                            nBytes = ReadCompressedInt64Data(mat, matvar->internal->z,
                                                             (mat_int64_t *)complex_data->Im,
                                                             packed_type, sparse->ndata);
#endif
                            break;
                        case MAT_T_UINT64:
#ifdef HAVE_MAT_UINT64_T
                            nBytes = ReadCompressedUInt64Data(mat, matvar->internal->z,
                                                              (mat_uint64_t *)complex_data->Im,
                                                              packed_type, sparse->ndata);
#endif
                            break;
                        case MAT_T_INT32:
                            nBytes = ReadCompressedInt32Data(mat, matvar->internal->z,
                                                             (mat_int32_t *)complex_data->Im,
                                                             packed_type, sparse->ndata);
                            break;
                        case MAT_T_UINT32:
                            nBytes = ReadCompressedUInt32Data(mat, matvar->internal->z,
                                                              (mat_uint32_t *)complex_data->Im,
                                                              packed_type, sparse->ndata);
                            break;
                        case MAT_T_INT16:
                            nBytes = ReadCompressedInt16Data(mat, matvar->internal->z,
                                                             (mat_int16_t *)complex_data->Im,
                                                             packed_type, sparse->ndata);
                            break;
                        case MAT_T_UINT16:
                            nBytes = ReadCompressedUInt16Data(mat, matvar->internal->z,
                                                              (mat_uint16_t *)complex_data->Im,
                                                              packed_type, sparse->ndata);
                            break;
                        case MAT_T_INT8:
                            nBytes = ReadCompressedInt8Data(mat, matvar->internal->z,
                                                            (mat_int8_t *)complex_data->Im,
                                                            packed_type, sparse->ndata);
                            break;
                        case MAT_T_UINT8:
                            nBytes = ReadCompressedUInt8Data(mat, matvar->internal->z,
                                                             (mat_uint8_t *)complex_data->Im,
                                                             packed_type, sparse->ndata);
                            break;
                        default:
                            break;
                    }
#else  /* EXTENDED_SPARSE */
                    nBytes = ReadCompressedDoubleData(mat, matvar->internal->z,
                                                      (double *)complex_data->Im, packed_type,
                                                      sparse->ndata);
#endif /* EXTENDED_SPARSE */
                    if ( data_in_tag )
                        nBytes += 4;
                    if ( (nBytes % 8) != 0 )
                        err = InflateSkip(mat, matvar->internal->z, 8 - (nBytes % 8), NULL);
#endif /* HAVE_ZLIB */
                }
                sparse->data = complex_data;
            } else { /* isComplex */
                size_t nbytes = 0;
                err = Mul(&nbytes, sparse->ndata, Mat_SizeOf(matvar->data_type));
                if ( err ) {
                    Mat_Critical("Integer multiplication overflow");
                    break;
                }
                sparse->data = malloc(nbytes);
                if ( sparse->data == NULL ) {
                    err = MATIO_E_OUT_OF_MEMORY;
                    Mat_Critical("Couldn't allocate memory for the sparse data");
                    break;
                }
                if ( matvar->compression == MAT_COMPRESSION_NONE ) {
#if defined(EXTENDED_SPARSE)
                    switch ( matvar->data_type ) {
                        case MAT_T_DOUBLE:
                            nBytes = ReadDoubleData(mat, (double *)sparse->data, packed_type,
                                                    sparse->ndata);
                            break;
                        case MAT_T_SINGLE:
                            nBytes = ReadSingleData(mat, (float *)sparse->data, packed_type,
                                                    sparse->ndata);
                            break;
                        case MAT_T_INT64:
#ifdef HAVE_MAT_INT64_T
                            nBytes = ReadInt64Data(mat, (mat_int64_t *)sparse->data, packed_type,
                                                   sparse->ndata);
#endif
                            break;
                        case MAT_T_UINT64:
#ifdef HAVE_MAT_UINT64_T
                            nBytes = ReadUInt64Data(mat, (mat_uint64_t *)sparse->data, packed_type,
                                                    sparse->ndata);
#endif
                            break;
                        case MAT_T_INT32:
                            nBytes = ReadInt32Data(mat, (mat_int32_t *)sparse->data, packed_type,
                                                   sparse->ndata);
                            break;
                        case MAT_T_UINT32:
                            nBytes = ReadUInt32Data(mat, (mat_uint32_t *)sparse->data, packed_type,
                                                    sparse->ndata);
                            break;
                        case MAT_T_INT16:
                            nBytes = ReadInt16Data(mat, (mat_int16_t *)sparse->data, packed_type,
                                                   sparse->ndata);
                            break;
                        case MAT_T_UINT16:
                            nBytes = ReadUInt16Data(mat, (mat_uint16_t *)sparse->data, packed_type,
                                                    sparse->ndata);
                            break;
                        case MAT_T_INT8:
                            nBytes = ReadInt8Data(mat, (mat_int8_t *)sparse->data, packed_type,
                                                  sparse->ndata);
                            break;
                        case MAT_T_UINT8:
                            nBytes = ReadUInt8Data(mat, (mat_uint8_t *)sparse->data, packed_type,
                                                   sparse->ndata);
                            break;
                        default:
                            break;
                    }
#else
                    nBytes =
                        ReadDoubleData(mat, (double *)sparse->data, packed_type, sparse->ndata);
#endif
                    nBytes *= Mat_SizeOf(packed_type);
                    if ( data_in_tag )
                        nBytes += 4;
                    if ( (nBytes % 8) != 0 )
                        (void)fseek((FILE *)mat->fp, 8 - (nBytes % 8), SEEK_CUR);
#if HAVE_ZLIB
                } else if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
#if defined(EXTENDED_SPARSE)
                    switch ( matvar->data_type ) {
                        case MAT_T_DOUBLE:
                            nBytes = ReadCompressedDoubleData(mat, matvar->internal->z,
                                                              (double *)sparse->data, packed_type,
                                                              sparse->ndata);
                            break;
                        case MAT_T_SINGLE:
                            nBytes = ReadCompressedSingleData(mat, matvar->internal->z,
                                                              (float *)sparse->data, packed_type,
                                                              sparse->ndata);
                            break;
                        case MAT_T_INT64:
#ifdef HAVE_MAT_INT64_T
                            nBytes = ReadCompressedInt64Data(mat, matvar->internal->z,
                                                             (mat_int64_t *)sparse->data,
                                                             packed_type, sparse->ndata);
#endif
                            break;
                        case MAT_T_UINT64:
#ifdef HAVE_MAT_UINT64_T
                            nBytes = ReadCompressedUInt64Data(mat, matvar->internal->z,
                                                              (mat_uint64_t *)sparse->data,
                                                              packed_type, sparse->ndata);
#endif
                            break;
                        case MAT_T_INT32:
                            nBytes = ReadCompressedInt32Data(mat, matvar->internal->z,
                                                             (mat_int32_t *)sparse->data,
                                                             packed_type, sparse->ndata);
                            break;
                        case MAT_T_UINT32:
                            nBytes = ReadCompressedUInt32Data(mat, matvar->internal->z,
                                                              (mat_uint32_t *)sparse->data,
                                                              packed_type, sparse->ndata);
                            break;
                        case MAT_T_INT16:
                            nBytes = ReadCompressedInt16Data(mat, matvar->internal->z,
                                                             (mat_int16_t *)sparse->data,
                                                             packed_type, sparse->ndata);
                            break;
                        case MAT_T_UINT16:
                            nBytes = ReadCompressedUInt16Data(mat, matvar->internal->z,
                                                              (mat_uint16_t *)sparse->data,
                                                              packed_type, sparse->ndata);
                            break;
                        case MAT_T_INT8:
                            nBytes = ReadCompressedInt8Data(mat, matvar->internal->z,
                                                            (mat_int8_t *)sparse->data, packed_type,
                                                            sparse->ndata);
                            break;
                        case MAT_T_UINT8:
                            nBytes = ReadCompressedUInt8Data(mat, matvar->internal->z,
                                                             (mat_uint8_t *)sparse->data,
                                                             packed_type, sparse->ndata);
                            break;
                        default:
                            break;
                    }
#else  /* EXTENDED_SPARSE */
                    nBytes =
                        ReadCompressedDoubleData(mat, matvar->internal->z, (double *)sparse->data,
                                                 packed_type, sparse->ndata);
#endif /* EXTENDED_SPARSE */
                    if ( data_in_tag )
                        nBytes += 4;
                    if ( (nBytes % 8) != 0 )
                        err = InflateSkip(mat, matvar->internal->z, 8 - (nBytes % 8), NULL);
#endif /* HAVE_ZLIB */
                }
            }
            break;
        }
        case MAT_C_FUNCTION: {
            matvar_t **functions;
            size_t nfunctions = 0;

            if ( !matvar->nbytes || !matvar->data_size )
                break;
            nfunctions = matvar->nbytes / matvar->data_size;
            functions = (matvar_t **)matvar->data;
            if ( NULL != functions ) {
                size_t i;
                for ( i = 0; i < nfunctions; i++ ) {
                    err = Mat_VarRead5(mat, functions[i]);
                    if ( err )
                        break;
                }
            }
            /* FIXME: */
            matvar->data_type = MAT_T_FUNCTION;
            break;
        }
        case MAT_C_OBJECT:
            Mat_Warning("Mat_VarRead5: %d is not a supported class", matvar->class_type);
            break;
        default:
            err = MATIO_E_OPERATION_NOT_SUPPORTED;
            Mat_Critical("Mat_VarRead5: %d is not a supported class", matvar->class_type);
            break;
    }
    switch ( matvar->class_type ) {
        case MAT_C_DOUBLE:
        case MAT_C_SINGLE:
#ifdef HAVE_MAT_INT64_T
        case MAT_C_INT64:
#endif
#ifdef HAVE_MAT_UINT64_T
        case MAT_C_UINT64:
#endif
        case MAT_C_INT32:
        case MAT_C_UINT32:
        case MAT_C_INT16:
        case MAT_C_UINT16:
        case MAT_C_INT8:
        case MAT_C_UINT8:
            if ( matvar->isComplex ) {
                mat_complex_split_t *complex_data;

                err = Mul(&matvar->nbytes, nelems, matvar->data_size);
                if ( err ) {
                    Mat_Critical("Integer multiplication overflow");
                    break;
                }

                complex_data = ComplexMalloc(matvar->nbytes);
                if ( NULL == complex_data ) {
                    err = MATIO_E_OUT_OF_MEMORY;
                    Mat_Critical("Couldn't allocate memory for the complex data");
                    break;
                }

                err = Mat_VarReadNumeric5(mat, matvar, complex_data->Re, nelems);
                if ( err ) {
                    ComplexFree(complex_data);
                    break;
                }
                err = Mat_VarReadNumeric5(mat, matvar, complex_data->Im, nelems);
                if ( err ) {
                    ComplexFree(complex_data);
                    break;
                }
                matvar->data = complex_data;
            } else {
                void *data;
                err = Mul(&matvar->nbytes, nelems, matvar->data_size);
                if ( err ) {
                    Mat_Critical("Integer multiplication overflow");
                    break;
                }

                data = malloc(matvar->nbytes);
                if ( NULL == data ) {
                    err = MATIO_E_OUT_OF_MEMORY;
                    Mat_Critical("Couldn't allocate memory for the data");
                    break;
                }
                err = Mat_VarReadNumeric5(mat, matvar, data, nelems);
                if ( err ) {
                    free(data);
                    break;
                }
                matvar->data = data;
            }
        default:
            break;
    }
    (void)fseek((FILE *)mat->fp, fpos, SEEK_SET);

    return err;
}