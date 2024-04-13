WriteCompressedCharData(mat_t *mat, z_streamp z, void *data, size_t N, enum matio_types data_type)
{
    size_t data_size, byteswritten = 0, nbytes = 0;
    mat_uint32_t data_tag[2] = {0, 0};
    int buf_size = 1024;
    int err;
    mat_uint8_t buf[1024], pad[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    if ( mat == NULL || mat->fp == NULL )
        return 0;

    if ( data_type == MAT_T_UNKNOWN ) {
        data_size = Mat_SizeOf(MAT_T_UINT16);
    } else {
        data_size = Mat_SizeOf(data_type);
    }

    err = Mul(&nbytes, N, data_size);
    if ( err ) {
        return byteswritten;
    }

    switch ( data_type ) {
        case MAT_T_UINT8:
        case MAT_T_UINT16:
        case MAT_T_UTF8:
        case MAT_T_UTF16:
            data_tag[0] = MAT_T_UINT8 == data_type ? MAT_T_UTF8 : data_type;
            data_tag[1] = (mat_uint32_t)nbytes;
            z->next_in = ZLIB_BYTE_PTR(data_tag);
            z->avail_in = 8;
            do {
                z->next_out = buf;
                z->avail_out = buf_size;
                deflate(z, Z_NO_FLUSH);
                byteswritten += fwrite(buf, 1, buf_size - z->avail_out, (FILE *)mat->fp);
            } while ( z->avail_out == 0 );

            /* exit early if this is an empty data */
            if ( NULL == data || N < 1 )
                break;

            z->next_in = (Bytef *)data;
            z->avail_in = (mat_uint32_t)nbytes;
            do {
                z->next_out = buf;
                z->avail_out = buf_size;
                deflate(z, Z_NO_FLUSH);
                byteswritten += fwrite(buf, 1, buf_size - z->avail_out, (FILE *)mat->fp);
            } while ( z->avail_out == 0 );
            /* Add/Compress padding to pad to 8-byte boundary */
            if ( nbytes % 8 ) {
                z->next_in = pad;
                z->avail_in = 8 - (nbytes % 8);
                do {
                    z->next_out = buf;
                    z->avail_out = buf_size;
                    deflate(z, Z_NO_FLUSH);
                    byteswritten += fwrite(buf, 1, buf_size - z->avail_out, (FILE *)mat->fp);
                } while ( z->avail_out == 0 );
            }
            break;
        case MAT_T_UNKNOWN:
            /* Sometimes empty char data will have MAT_T_UNKNOWN, so just write a data tag */
            data_tag[0] = MAT_T_UINT16;
            data_tag[1] = (mat_uint32_t)nbytes;
            z->next_in = ZLIB_BYTE_PTR(data_tag);
            z->avail_in = 8;
            do {
                z->next_out = buf;
                z->avail_out = buf_size;
                deflate(z, Z_NO_FLUSH);
                byteswritten += fwrite(buf, 1, buf_size - z->avail_out, (FILE *)mat->fp);
            } while ( z->avail_out == 0 );
            break;
        default:
            break;
    }

    return byteswritten;
}