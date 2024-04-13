static void emit_binary_diff(FILE *file, mmfile_t *one, mmfile_t *two)
{
	fprintf(file, "GIT binary patch\n");
	emit_binary_diff_body(file, one, two);
	emit_binary_diff_body(file, two, one);
}