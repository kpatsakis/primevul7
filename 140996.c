Mat_VarReadData5(mat_t *mat, matvar_t *matvar, void *data, int *start, int *stride, int *edge)
{
    int err = MATIO_E_NO_ERROR, real_bytes = 0;
    mat_uint32_t tag[2] = {0, 0};
#if HAVE_ZLIB
    z_stream z;
#endif

    (void)fseek((FILE *)mat->fp, matvar->internal->datapos, SEEK_SET);
    if ( matvar->compression == MAT_COMPRESSION_NONE ) {
        err = Read(tag, 4, 2, (FILE *)mat->fp, NULL);
        if ( err ) {
            return err;
        }
        if ( mat->byteswap ) {
            (void)Mat_uint32Swap(tag);
            (void)Mat_uint32Swap(tag + 1);
        }
        matvar->data_type = TYPE_FROM_TAG(tag[0]);
        if ( tag[0] & 0xffff0000 ) { /* Data is packed in the tag */
            (void)fseek((FILE *)mat->fp, -4, SEEK_CUR);
            real_bytes = 4 + (tag[0] >> 16);
        } else {
            real_bytes = 8 + tag[1];
        }
#if HAVE_ZLIB
    } else if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
        if ( NULL != matvar->internal->data ) {
            /* Data already read in ReadNextStructField or ReadNextCell */
            if ( matvar->isComplex ) {
                mat_complex_split_t *ci, *co;

                co = (mat_complex_split_t *)data;
                ci = (mat_complex_split_t *)matvar->internal->data;
                err = GetDataSlab(ci->Re, co->Re, matvar->class_type, matvar->data_type,
                                  matvar->dims, start, stride, edge, matvar->rank, matvar->nbytes);
                if ( MATIO_E_NO_ERROR == err )
                    err = GetDataSlab(ci->Im, co->Im, matvar->class_type, matvar->data_type,
                                      matvar->dims, start, stride, edge, matvar->rank,
                                      matvar->nbytes);
                return err;
            } else {
                return GetDataSlab(matvar->internal->data, data, matvar->class_type,
                                   matvar->data_type, matvar->dims, start, stride, edge,
                                   matvar->rank, matvar->nbytes);
            }
        }

        err = inflateCopy(&z, matvar->internal->z);
        if ( err != Z_OK ) {
            Mat_Critical("inflateCopy returned error %s", zError(err));
            return MATIO_E_FILE_FORMAT_VIOLATION;
        }
        z.avail_in = 0;
        err = Inflate(mat, &z, tag, 4, NULL);
        if ( err ) {
            return err;
        }
        if ( mat->byteswap ) {
            (void)Mat_uint32Swap(tag);
        }
        matvar->data_type = TYPE_FROM_TAG(tag[0]);
        if ( !(tag[0] & 0xffff0000) ) { /* Data is NOT packed in the tag */
            err = Inflate(mat, &z, tag + 1, 4, NULL);
            if ( err ) {
                return err;
            }
            if ( mat->byteswap ) {
                (void)Mat_uint32Swap(tag + 1);
            }
            real_bytes = 8 + tag[1];
        } else {
            real_bytes = 4 + (tag[0] >> 16);
        }
#endif
    }
    if ( real_bytes % 8 )
        real_bytes += (8 - (real_bytes % 8));

    if ( matvar->rank == 2 ) {
        if ( (size_t)stride[0] * (edge[0] - 1) + start[0] + 1 > matvar->dims[0] )
            err = MATIO_E_BAD_ARGUMENT;
        else if ( (size_t)stride[1] * (edge[1] - 1) + start[1] + 1 > matvar->dims[1] )
            err = MATIO_E_BAD_ARGUMENT;
        else if ( matvar->compression == MAT_COMPRESSION_NONE ) {
            if ( matvar->isComplex ) {
                mat_complex_split_t *complex_data = (mat_complex_split_t *)data;

                ReadDataSlab2(mat, complex_data->Re, matvar->class_type, matvar->data_type,
                              matvar->dims, start, stride, edge);
                (void)fseek((FILE *)mat->fp, matvar->internal->datapos + real_bytes, SEEK_SET);
                err = Read(tag, 4, 2, (FILE *)mat->fp, NULL);
                if ( err ) {
                    return err;
                }
                if ( mat->byteswap ) {
                    (void)Mat_uint32Swap(tag);
                    (void)Mat_uint32Swap(tag + 1);
                }
                matvar->data_type = TYPE_FROM_TAG(tag[0]);
                if ( tag[0] & 0xffff0000 ) { /* Data is packed in the tag */
                    (void)fseek((FILE *)mat->fp, -4, SEEK_CUR);
                }
                ReadDataSlab2(mat, complex_data->Im, matvar->class_type, matvar->data_type,
                              matvar->dims, start, stride, edge);
            } else {
                ReadDataSlab2(mat, data, matvar->class_type, matvar->data_type, matvar->dims, start,
                              stride, edge);
            }
        }
#if HAVE_ZLIB
        else if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
            if ( matvar->isComplex ) {
                mat_complex_split_t *complex_data = (mat_complex_split_t *)data;

                ReadCompressedDataSlab2(mat, &z, complex_data->Re, matvar->class_type,
                                        matvar->data_type, matvar->dims, start, stride, edge);

                (void)fseek((FILE *)mat->fp, matvar->internal->datapos, SEEK_SET);

                /* Reset zlib knowledge to before reading real tag */
                inflateEnd(&z);
                err = inflateCopy(&z, matvar->internal->z);
                if ( err != Z_OK ) {
                    Mat_Critical("inflateCopy returned error %s", zError(err));
                    return MATIO_E_FILE_FORMAT_VIOLATION;
                }
                InflateSkip(mat, &z, real_bytes, NULL);
                z.avail_in = 0;
                err = Inflate(mat, &z, tag, 4, NULL);
                if ( err ) {
                    return err;
                }
                if ( mat->byteswap ) {
                    (void)Mat_uint32Swap(tag);
                }
                matvar->data_type = TYPE_FROM_TAG(tag[0]);
                if ( !(tag[0] & 0xffff0000) ) { /*Data is NOT packed in the tag*/
                    InflateSkip(mat, &z, 4, NULL);
                }
                ReadCompressedDataSlab2(mat, &z, complex_data->Im, matvar->class_type,
                                        matvar->data_type, matvar->dims, start, stride, edge);
            } else {
                ReadCompressedDataSlab2(mat, &z, data, matvar->class_type, matvar->data_type,
                                        matvar->dims, start, stride, edge);
            }
            inflateEnd(&z);
        }
#endif
    } else {
        if ( matvar->compression == MAT_COMPRESSION_NONE ) {
            if ( matvar->isComplex ) {
                mat_complex_split_t *complex_data = (mat_complex_split_t *)data;

                ReadDataSlabN(mat, complex_data->Re, matvar->class_type, matvar->data_type,
                              matvar->rank, matvar->dims, start, stride, edge);

                (void)fseek((FILE *)mat->fp, matvar->internal->datapos + real_bytes, SEEK_SET);
                err = Read(tag, 4, 2, (FILE *)mat->fp, NULL);
                if ( err ) {
                    return err;
                }
                if ( mat->byteswap ) {
                    (void)Mat_uint32Swap(tag);
                    (void)Mat_uint32Swap(tag + 1);
                }
                matvar->data_type = TYPE_FROM_TAG(tag[0]);
                if ( tag[0] & 0xffff0000 ) { /* Data is packed in the tag */
                    (void)fseek((FILE *)mat->fp, -4, SEEK_CUR);
                }
                ReadDataSlabN(mat, complex_data->Im, matvar->class_type, matvar->data_type,
                              matvar->rank, matvar->dims, start, stride, edge);
            } else {
                ReadDataSlabN(mat, data, matvar->class_type, matvar->data_type, matvar->rank,
                              matvar->dims, start, stride, edge);
            }
        }
#if HAVE_ZLIB
        else if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
            if ( matvar->isComplex ) {
                mat_complex_split_t *complex_data = (mat_complex_split_t *)data;

                ReadCompressedDataSlabN(mat, &z, complex_data->Re, matvar->class_type,
                                        matvar->data_type, matvar->rank, matvar->dims, start,
                                        stride, edge);

                (void)fseek((FILE *)mat->fp, matvar->internal->datapos, SEEK_SET);
                /* Reset zlib knowledge to before reading real tag */
                inflateEnd(&z);
                err = inflateCopy(&z, matvar->internal->z);
                if ( err != Z_OK ) {
                    Mat_Critical("inflateCopy returned error %s", zError(err));
                    return MATIO_E_FILE_FORMAT_VIOLATION;
                }
                InflateSkip(mat, &z, real_bytes, NULL);
                z.avail_in = 0;
                err = Inflate(mat, &z, tag, 4, NULL);
                if ( err ) {
                    return err;
                }
                if ( mat->byteswap ) {
                    (void)Mat_uint32Swap(tag);
                }
                matvar->data_type = TYPE_FROM_TAG(tag[0]);
                if ( !(tag[0] & 0xffff0000) ) { /*Data is NOT packed in the tag*/
                    InflateSkip(mat, &z, 4, NULL);
                }
                ReadCompressedDataSlabN(mat, &z, complex_data->Im, matvar->class_type,
                                        matvar->data_type, matvar->rank, matvar->dims, start,
                                        stride, edge);
            } else {
                ReadCompressedDataSlabN(mat, &z, data, matvar->class_type, matvar->data_type,
                                        matvar->rank, matvar->dims, start, stride, edge);
            }
            inflateEnd(&z);
        }
#endif
    }
    if ( err == MATIO_E_NO_ERROR ) {
        matvar->data_type = ClassType2DataType(matvar->class_type);
        matvar->data_size = Mat_SizeOfClass(matvar->class_type);
    }
    return err;
}