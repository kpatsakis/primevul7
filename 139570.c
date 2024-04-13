void jas_matrix_destroy(jas_matrix_t *matrix)
{
	if (matrix->data_) {
		assert(!(matrix->flags_ & JAS_MATRIX_REF));
		jas_free(matrix->data_);
		matrix->data_ = 0;
	}
	if (matrix->rows_) {
		jas_free(matrix->rows_);
		matrix->rows_ = 0;
	}
	jas_free(matrix);
}