WriteCharData(mat_t *mat, void *data, size_t N, enum matio_types data_type)
{
    mat_uint32_t nBytes = 0;
    size_t nbytes = 0, i;
    size_t byteswritten = 0;
    const mat_uint8_t pad1 = 0;
    int err;

    switch ( data_type ) {
        case MAT_T_UINT8:
        case MAT_T_UINT16:
        case MAT_T_UTF8:
        case MAT_T_UTF16: {
            data_type = MAT_T_UINT8 == data_type ? MAT_T_UTF8 : data_type;
            err = Mul(&nbytes, N, Mat_SizeOf(data_type));
            if ( err ) {
                return 0;
            }
            nBytes = (mat_uint32_t)nbytes;
            fwrite(&data_type, 4, 1, (FILE *)mat->fp);
            fwrite(&nBytes, 4, 1, (FILE *)mat->fp);
            if ( NULL != data && N > 0 )
                fwrite(data, 1, nbytes, (FILE *)mat->fp);
            if ( nBytes % 8 ) {
                for ( i = nbytes % 8; i < 8; i++ )
                    fwrite(&pad1, 1, 1, (FILE *)mat->fp);
            }
            break;
        }
        case MAT_T_INT8: {
            mat_int8_t *ptr;
            mat_uint16_t c;

            /* Matlab can't read MAT_C_CHAR as int8, needs uint16 */
            data_type = MAT_T_UINT16;
            err = Mul(&nbytes, N, Mat_SizeOf(data_type));
            if ( err ) {
                return 0;
            }
            nBytes = (mat_uint32_t)nbytes;
            fwrite(&data_type, 4, 1, (FILE *)mat->fp);
            fwrite(&nBytes, 4, 1, (FILE *)mat->fp);
            ptr = (mat_int8_t *)data;
            if ( NULL == data )
                break;
            for ( i = 0; i < N; i++ ) {
                c = (mat_uint16_t) * (char *)ptr;
                fwrite(&c, 2, 1, (FILE *)mat->fp);
                ptr++;
            }
            if ( nbytes % 8 )
                for ( i = nbytes % 8; i < 8; i++ )
                    fwrite(&pad1, 1, 1, (FILE *)mat->fp);
            break;
        }
        case MAT_T_UNKNOWN: {
            /* Sometimes empty char data will have MAT_T_UNKNOWN, so just write
             * a data tag
             */
            data_type = MAT_T_UINT16;
            err = Mul(&nbytes, N, Mat_SizeOf(data_type));
            if ( err ) {
                return 0;
            }
            nBytes = (mat_uint32_t)nbytes;
            fwrite(&data_type, 4, 1, (FILE *)mat->fp);
            fwrite(&nBytes, 4, 1, (FILE *)mat->fp);
            break;
        }
        default:
            nbytes = 0;
            break;
    }
    byteswritten += nbytes;
    return byteswritten;
}