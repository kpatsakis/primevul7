Mat_VarReadDataLinear5(mat_t *mat, matvar_t *matvar, void *data, int start, int stride, int edge)
{
    int err = MATIO_E_NO_ERROR, real_bytes = 0;
    mat_uint32_t tag[2] = {0, 0};
#if HAVE_ZLIB
    z_stream z;
#endif
    size_t nelems = 1;

    if ( mat->version == MAT_FT_MAT4 )
        return -1;
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
        matvar->data_type = (enum matio_types)(tag[0] & 0x000000ff);
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
                err = GetDataLinear(ci->Re, co->Re, matvar->class_type, matvar->data_type, start,
                                    stride, edge);
                if ( err == MATIO_E_NO_ERROR )
                    err = GetDataLinear(ci->Im, co->Im, matvar->class_type, matvar->data_type,
                                        start, stride, edge);
                return err;
            } else {
                return GetDataLinear(matvar->internal->data, data, matvar->class_type,
                                     matvar->data_type, start, stride, edge);
            }
        }

        matvar->internal->z->avail_in = 0;
        err = inflateCopy(&z, matvar->internal->z);
        if ( err != Z_OK ) {
            Mat_Critical("inflateCopy returned error %s", zError(err));
            return MATIO_E_FILE_FORMAT_VIOLATION;
        }
        err = Inflate(mat, &z, tag, 4, NULL);
        if ( err ) {
            return err;
        }
        if ( mat->byteswap ) {
            (void)Mat_uint32Swap(tag);
            (void)Mat_uint32Swap(tag + 1);
        }
        matvar->data_type = (enum matio_types)(tag[0] & 0x000000ff);
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

    err = Mat_MulDims(matvar, &nelems);
    if ( err ) {
        Mat_Critical("Integer multiplication overflow");
        return err;
    }

    if ( (size_t)stride * (edge - 1) + start + 1 > nelems ) {
        err = MATIO_E_BAD_ARGUMENT;
    } else if ( matvar->compression == MAT_COMPRESSION_NONE ) {
        if ( matvar->isComplex ) {
            mat_complex_split_t *complex_data = (mat_complex_split_t *)data;

            ReadDataSlab1(mat, complex_data->Re, matvar->class_type, matvar->data_type, start,
                          stride, edge);
            (void)fseek((FILE *)mat->fp, matvar->internal->datapos + real_bytes, SEEK_SET);
            err = Read(tag, 4, 2, (FILE *)mat->fp, NULL);
            if ( err ) {
                return err;
            }
            if ( mat->byteswap ) {
                (void)Mat_uint32Swap(tag);
                (void)Mat_uint32Swap(tag + 1);
            }
            matvar->data_type = (enum matio_types)(tag[0] & 0x000000ff);
            if ( tag[0] & 0xffff0000 ) { /* Data is packed in the tag */
                (void)fseek((FILE *)mat->fp, -4, SEEK_CUR);
            }
            ReadDataSlab1(mat, complex_data->Im, matvar->class_type, matvar->data_type, start,
                          stride, edge);
        } else {
            ReadDataSlab1(mat, data, matvar->class_type, matvar->data_type, start, stride, edge);
        }
#if HAVE_ZLIB
    } else if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
        if ( matvar->isComplex ) {
            mat_complex_split_t *complex_data = (mat_complex_split_t *)data;

            ReadCompressedDataSlab1(mat, &z, complex_data->Re, matvar->class_type,
                                    matvar->data_type, start, stride, edge);

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
            matvar->data_type = (enum matio_types)(tag[0] & 0x000000ff);
            if ( !(tag[0] & 0xffff0000) ) { /*Data is NOT packed in the tag*/
                InflateSkip(mat, &z, 4, NULL);
            }
            ReadCompressedDataSlab1(mat, &z, complex_data->Im, matvar->class_type,
                                    matvar->data_type, start, stride, edge);
        } else {
            ReadCompressedDataSlab1(mat, &z, data, matvar->class_type, matvar->data_type, start,
                                    stride, edge);
        }
        inflateEnd(&z);
#endif
    }

    matvar->data_type = ClassType2DataType(matvar->class_type);
    matvar->data_size = Mat_SizeOfClass(matvar->class_type);

    return err;
}