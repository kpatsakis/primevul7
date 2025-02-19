WriteStructField(mat_t *mat, matvar_t *matvar)
{
    mat_uint32_t array_flags;
    const mat_uint32_t array_name_type = MAT_T_INT8;
    int array_flags_type = MAT_T_UINT32, dims_array_type = MAT_T_INT32;
    int array_flags_size = 8, matrix_type = MAT_T_MATRIX;
    const mat_uint32_t pad4 = 0;
    int nBytes, i, nzmax = 0;
    long start = 0, end = 0;

    if ( mat == NULL )
        return MATIO_E_BAD_ARGUMENT;

    if ( NULL == matvar ) {
        size_t dims[2] = {0, 0};
        Mat_WriteEmptyVariable5(mat, NULL, 2, dims);
        return MATIO_E_NO_ERROR;
    }

    fwrite(&matrix_type, 4, 1, (FILE *)mat->fp);
    fwrite(&pad4, 4, 1, (FILE *)mat->fp);
    if ( MAT_C_EMPTY == matvar->class_type ) {
        /* exit early if this is an empty data */
        return MATIO_E_NO_ERROR;
    }
    start = ftell((FILE *)mat->fp);

    /* Array Flags */
    array_flags = matvar->class_type & CLASS_TYPE_MASK;
    if ( matvar->isComplex )
        array_flags |= MAT_F_COMPLEX;
    if ( matvar->isGlobal )
        array_flags |= MAT_F_GLOBAL;
    if ( matvar->isLogical )
        array_flags |= MAT_F_LOGICAL;
    if ( matvar->class_type == MAT_C_SPARSE )
        nzmax = ((mat_sparse_t *)matvar->data)->nzmax;

    if ( mat->byteswap )
        array_flags = Mat_int32Swap((mat_int32_t *)&array_flags);
    fwrite(&array_flags_type, 4, 1, (FILE *)mat->fp);
    fwrite(&array_flags_size, 4, 1, (FILE *)mat->fp);
    fwrite(&array_flags, 4, 1, (FILE *)mat->fp);
    fwrite(&nzmax, 4, 1, (FILE *)mat->fp);
    /* Rank and Dimension */
    nBytes = matvar->rank * 4;
    fwrite(&dims_array_type, 4, 1, (FILE *)mat->fp);
    fwrite(&nBytes, 4, 1, (FILE *)mat->fp);
    for ( i = 0; i < matvar->rank; i++ ) {
        mat_int32_t dim;
        dim = matvar->dims[i];
        fwrite(&dim, 4, 1, (FILE *)mat->fp);
    }
    if ( matvar->rank % 2 != 0 )
        fwrite(&pad4, 4, 1, (FILE *)mat->fp);

    /* Name of variable */
    fwrite(&array_name_type, 4, 1, (FILE *)mat->fp);
    fwrite(&pad4, 4, 1, (FILE *)mat->fp);

    WriteType(mat, matvar);
    end = ftell((FILE *)mat->fp);
    if ( start != -1L && end != -1L ) {
        nBytes = (int)(end - start);
        (void)fseek((FILE *)mat->fp, (long)-(nBytes + 4), SEEK_CUR);
        fwrite(&nBytes, 4, 1, (FILE *)mat->fp);
        (void)fseek((FILE *)mat->fp, end, SEEK_SET);
    } else {
        Mat_Critical("Couldn't determine file position");
    }

    return MATIO_E_NO_ERROR;
}