static void jpc_dequantize(jas_matrix_t *x, jpc_fix_t absstepsize)
{
	int i;
	int j;
	int t;

	assert(absstepsize >= 0);
	if (absstepsize == jpc_inttofix(1)) {
		return;
	}

	for (i = 0; i < jas_matrix_numrows(x); ++i) {
		for (j = 0; j < jas_matrix_numcols(x); ++j) {
			t = jas_matrix_get(x, i, j);
			if (t) {
				t = jpc_fix_mul(t, absstepsize);
			} else {
				t = 0;
			}
			jas_matrix_set(x, i, j, t);
		}
	}

}