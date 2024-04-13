ReadNextCell(mat_t *mat, matvar_t *matvar)
{
    size_t bytesread = 0, i;
    int err;
    matvar_t **cells = NULL;
    size_t nelems = 1;

    err = Mat_MulDims(matvar, &nelems);
    if ( err ) {
        Mat_Critical("Integer multiplication overflow");
        return bytesread;
    }
    matvar->data_size = sizeof(matvar_t *);
    err = Mul(&matvar->nbytes, nelems, matvar->data_size);
    if ( err ) {
        Mat_Critical("Integer multiplication overflow");
        return bytesread;
    }

    matvar->data = calloc(nelems, matvar->data_size);
    if ( NULL == matvar->data ) {
        if ( NULL != matvar->name )
            Mat_Critical("Couldn't allocate memory for %s->data", matvar->name);
        return bytesread;
    }
    cells = (matvar_t **)matvar->data;

    if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
#if HAVE_ZLIB
        mat_uint32_t uncomp_buf[16];
        mat_uint32_t nBytes;
        mat_uint32_t array_flags;

        memset(&uncomp_buf, 0, sizeof(uncomp_buf));
        for ( i = 0; i < nelems; i++ ) {
            cells[i] = Mat_VarCalloc();
            if ( NULL == cells[i] ) {
                Mat_Critical("Couldn't allocate memory for cell %zu", i);
                continue;
            }

            /* Read variable tag for cell */
            uncomp_buf[0] = 0;
            uncomp_buf[1] = 0;
            err = Inflate(mat, matvar->internal->z, uncomp_buf, 8, &bytesread);
            if ( err ) {
                Mat_VarFree(cells[i]);
                cells[i] = NULL;
                break;
            }
            if ( mat->byteswap ) {
                (void)Mat_uint32Swap(uncomp_buf);
                (void)Mat_uint32Swap(uncomp_buf + 1);
            }
            nBytes = uncomp_buf[1];
            if ( 0 == nBytes ) {
                /* Empty cell: Memory optimization */
                free(cells[i]->internal);
                cells[i]->internal = NULL;
                continue;
            } else if ( uncomp_buf[0] != MAT_T_MATRIX ) {
                Mat_VarFree(cells[i]);
                cells[i] = NULL;
                Mat_Critical("cells[%zu], Uncompressed type not MAT_T_MATRIX", i);
                break;
            }
            cells[i]->compression = MAT_COMPRESSION_ZLIB;
            err = Inflate(mat, matvar->internal->z, uncomp_buf, 16, &bytesread);
            if ( err ) {
                Mat_VarFree(cells[i]);
                cells[i] = NULL;
                break;
            }
            nBytes -= 16;
            if ( mat->byteswap ) {
                (void)Mat_uint32Swap(uncomp_buf);
                (void)Mat_uint32Swap(uncomp_buf + 1);
                (void)Mat_uint32Swap(uncomp_buf + 2);
                (void)Mat_uint32Swap(uncomp_buf + 3);
            }
            /* Array Flags */
            if ( uncomp_buf[0] == MAT_T_UINT32 ) {
                array_flags = uncomp_buf[2];
                cells[i]->class_type = CLASS_FROM_ARRAY_FLAGS(array_flags);
                cells[i]->isComplex = (array_flags & MAT_F_COMPLEX);
                cells[i]->isGlobal = (array_flags & MAT_F_GLOBAL);
                cells[i]->isLogical = (array_flags & MAT_F_LOGICAL);
                if ( cells[i]->class_type == MAT_C_SPARSE ) {
                    /* Need to find a more appropriate place to store nzmax */
                    cells[i]->nbytes = uncomp_buf[3];
                }
            } else {
                Mat_Critical("Expected MAT_T_UINT32 for array tags, got %d", uncomp_buf[0]);
                InflateSkip(mat, matvar->internal->z, nBytes, &bytesread);
            }
            if ( cells[i]->class_type != MAT_C_OPAQUE ) {
                mat_uint32_t *dims = NULL;
                int do_clean = 0;
                err = InflateRankDims(mat, matvar->internal->z, uncomp_buf, sizeof(uncomp_buf),
                                      &dims, &bytesread);
                if ( NULL == dims ) {
                    dims = uncomp_buf + 2;
                } else {
                    do_clean = 1;
                }
                if ( err ) {
                    if ( do_clean ) {
                        free(dims);
                    }
                    Mat_VarFree(cells[i]);
                    cells[i] = NULL;
                    break;
                }
                nBytes -= 8;
                if ( mat->byteswap ) {
                    (void)Mat_uint32Swap(uncomp_buf);
                    (void)Mat_uint32Swap(uncomp_buf + 1);
                }
                /* Rank and Dimension */
                if ( uncomp_buf[0] == MAT_T_INT32 ) {
                    int j;
                    size_t size;
                    cells[i]->rank = uncomp_buf[1];
                    nBytes -= cells[i]->rank;
                    cells[i]->rank /= 4;
                    if ( 0 == do_clean && cells[i]->rank > 13 ) {
                        int rank = cells[i]->rank;
                        cells[i]->rank = 0;
                        Mat_Critical("%d is not a valid rank", rank);
                        continue;
                    }
                    err = Mul(&size, cells[i]->rank, sizeof(*cells[i]->dims));
                    if ( err ) {
                        if ( do_clean ) {
                            free(dims);
                        }
                        Mat_VarFree(cells[i]);
                        cells[i] = NULL;
                        Mat_Critical("Integer multiplication overflow");
                        continue;
                    }
                    cells[i]->dims = (size_t *)malloc(size);
                    if ( mat->byteswap ) {
                        for ( j = 0; j < cells[i]->rank; j++ )
                            cells[i]->dims[j] = Mat_uint32Swap(dims + j);
                    } else {
                        for ( j = 0; j < cells[i]->rank; j++ )
                            cells[i]->dims[j] = dims[j];
                    }
                    if ( cells[i]->rank % 2 != 0 )
                        nBytes -= 4;
                }
                if ( do_clean ) {
                    free(dims);
                }
                /* Variable name tag */
                err = Inflate(mat, matvar->internal->z, uncomp_buf, 8, &bytesread);
                if ( err ) {
                    Mat_VarFree(cells[i]);
                    cells[i] = NULL;
                    break;
                }
                nBytes -= 8;
                if ( mat->byteswap ) {
                    (void)Mat_uint32Swap(uncomp_buf);
                    (void)Mat_uint32Swap(uncomp_buf + 1);
                }
                /* Handle cell elements written with a variable name */
                if ( uncomp_buf[1] > 0 ) {
                    /* Name of variable */
                    if ( uncomp_buf[0] == MAT_T_INT8 ) { /* Name not in tag */
                        mat_uint32_t len = uncomp_buf[1];

                        if ( len % 8 > 0 ) {
                            if ( len < UINT32_MAX - 8 + (len % 8) )
                                len = len + 8 - (len % 8);
                            else {
                                Mat_VarFree(cells[i]);
                                cells[i] = NULL;
                                break;
                            }
                        }
                        cells[i]->name = (char *)malloc(len + 1);
                        nBytes -= len;
                        if ( NULL != cells[i]->name ) {
                            /* Variable name */
                            err =
                                Inflate(mat, matvar->internal->z, cells[i]->name, len, &bytesread);
                            if ( err ) {
                                Mat_VarFree(cells[i]);
                                cells[i] = NULL;
                                break;
                            }
                            cells[i]->name[len] = '\0';
                        }
                    } else {
                        mat_uint32_t len = (uncomp_buf[0] & 0xffff0000) >> 16;
                        if ( ((uncomp_buf[0] & 0x0000ffff) == MAT_T_INT8) && len > 0 && len <= 4 ) {
                            /* Name packed in tag */
                            cells[i]->name = (char *)malloc(len + 1);
                            if ( NULL != cells[i]->name ) {
                                memcpy(cells[i]->name, uncomp_buf + 1, len);
                                cells[i]->name[len] = '\0';
                            }
                        }
                    }
                }
                cells[i]->internal->z = (z_streamp)calloc(1, sizeof(z_stream));
                if ( cells[i]->internal->z != NULL ) {
                    err = inflateCopy(cells[i]->internal->z, matvar->internal->z);
                    if ( err == Z_OK ) {
                        cells[i]->internal->datapos = ftell((FILE *)mat->fp);
                        if ( cells[i]->internal->datapos != -1L ) {
                            cells[i]->internal->datapos -= matvar->internal->z->avail_in;
                            if ( cells[i]->class_type == MAT_C_STRUCT )
                                bytesread += ReadNextStructField(mat, cells[i]);
                            else if ( cells[i]->class_type == MAT_C_CELL )
                                bytesread += ReadNextCell(mat, cells[i]);
                            else if ( nBytes <= (1 << MAX_WBITS) ) {
                                /* Memory optimization: Read data if less in size
                                   than the zlib inflate state (approximately) */
                                err = Mat_VarRead5(mat, cells[i]);
                                cells[i]->internal->data = cells[i]->data;
                                cells[i]->data = NULL;
                            }
                            (void)fseek((FILE *)mat->fp, cells[i]->internal->datapos, SEEK_SET);
                        } else {
                            Mat_Critical("Couldn't determine file position");
                        }
                        if ( cells[i]->internal->data != NULL ||
                             cells[i]->class_type == MAT_C_STRUCT ||
                             cells[i]->class_type == MAT_C_CELL ) {
                            /* Memory optimization: Free inflate state */
                            inflateEnd(cells[i]->internal->z);
                            free(cells[i]->internal->z);
                            cells[i]->internal->z = NULL;
                        }
                    } else {
                        Mat_Critical("inflateCopy returned error %s", zError(err));
                    }
                } else {
                    Mat_Critical("Couldn't allocate memory");
                }
            }
            InflateSkip(mat, matvar->internal->z, nBytes, &bytesread);
        }
#else
        Mat_Critical("Not compiled with zlib support");
#endif

    } else {
        mat_uint32_t buf[6] = {0, 0, 0, 0, 0, 0};
        mat_uint32_t nBytes;
        mat_uint32_t array_flags;

        for ( i = 0; i < nelems; i++ ) {
            size_t nbytes = 0;
            mat_uint32_t name_len;
            cells[i] = Mat_VarCalloc();
            if ( NULL == cells[i] ) {
                Mat_Critical("Couldn't allocate memory for cell %zu", i);
                continue;
            }

            /* Read variable tag for cell */
            err = Read(buf, 4, 2, (FILE *)mat->fp, &nbytes);

            /* Empty cells at the end of a file may cause an EOF */
            if ( 0 == err && 0 == nbytes )
                continue;
            bytesread += nbytes;
            if ( err ) {
                Mat_VarFree(cells[i]);
                cells[i] = NULL;
                break;
            }
            if ( mat->byteswap ) {
                (void)Mat_uint32Swap(buf);
                (void)Mat_uint32Swap(buf + 1);
            }
            nBytes = buf[1];
            if ( 0 == nBytes ) {
                /* Empty cell: Memory optimization */
                free(cells[i]->internal);
                cells[i]->internal = NULL;
                continue;
            } else if ( buf[0] != MAT_T_MATRIX ) {
                Mat_VarFree(cells[i]);
                cells[i] = NULL;
                Mat_Critical("cells[%zu] not MAT_T_MATRIX, fpos = %ld", i, ftell((FILE *)mat->fp));
                break;
            }

            /* Read array flags and the dimensions tag */
            err = Read(buf, 4, 6, (FILE *)mat->fp, &bytesread);
            if ( err ) {
                Mat_VarFree(cells[i]);
                cells[i] = NULL;
                break;
            }
            if ( mat->byteswap ) {
                (void)Mat_uint32Swap(buf);
                (void)Mat_uint32Swap(buf + 1);
                (void)Mat_uint32Swap(buf + 2);
                (void)Mat_uint32Swap(buf + 3);
                (void)Mat_uint32Swap(buf + 4);
                (void)Mat_uint32Swap(buf + 5);
            }
            nBytes -= 24;
            /* Array flags */
            if ( buf[0] == MAT_T_UINT32 ) {
                array_flags = buf[2];
                cells[i]->class_type = CLASS_FROM_ARRAY_FLAGS(array_flags);
                cells[i]->isComplex = (array_flags & MAT_F_COMPLEX);
                cells[i]->isGlobal = (array_flags & MAT_F_GLOBAL);
                cells[i]->isLogical = (array_flags & MAT_F_LOGICAL);
                if ( cells[i]->class_type == MAT_C_SPARSE ) {
                    /* Need to find a more appropriate place to store nzmax */
                    cells[i]->nbytes = buf[3];
                }
            }
            /* Rank and dimension */
            nbytes = 0;
            err = ReadRankDims(mat, cells[i], (enum matio_types)buf[4], buf[5], &nbytes);
            bytesread += nbytes;
            nBytes -= nbytes;
            if ( err ) {
                Mat_VarFree(cells[i]);
                cells[i] = NULL;
                break;
            }
            /* Variable name tag */
            if ( 0 != Read(buf, 1, 8, (FILE *)mat->fp, &bytesread) ) {
                Mat_VarFree(cells[i]);
                cells[i] = NULL;
                break;
            }
            nBytes -= 8;
            if ( mat->byteswap ) {
                (void)Mat_uint32Swap(buf);
                (void)Mat_uint32Swap(buf + 1);
            }
            name_len = 0;
            if ( buf[1] > 0 ) {
                /* Name of variable */
                if ( buf[0] == MAT_T_INT8 ) { /* Name not in tag */
                    name_len = buf[1];
                    if ( name_len % 8 > 0 ) {
                        if ( name_len < UINT32_MAX - 8 + (name_len % 8) ) {
                            name_len = name_len + 8 - (name_len % 8);
                        } else {
                            Mat_VarFree(cells[i]);
                            cells[i] = NULL;
                            break;
                        }
                    }
                    nBytes -= name_len;
                    (void)fseek((FILE *)mat->fp, name_len, SEEK_CUR);
                }
            }
            cells[i]->internal->datapos = ftell((FILE *)mat->fp);
            if ( cells[i]->internal->datapos != -1L ) {
                if ( cells[i]->class_type == MAT_C_STRUCT )
                    bytesread += ReadNextStructField(mat, cells[i]);
                if ( cells[i]->class_type == MAT_C_CELL )
                    bytesread += ReadNextCell(mat, cells[i]);
                (void)fseek((FILE *)mat->fp, cells[i]->internal->datapos + nBytes, SEEK_SET);
            } else {
                Mat_Critical("Couldn't determine file position");
            }
        }
    }

    return bytesread;
}