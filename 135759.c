static void print_line_count(FILE *file, int count)
{
	switch (count) {
	case 0:
		fprintf(file, "0,0");
		break;
	case 1:
		fprintf(file, "1");
		break;
	default:
		fprintf(file, "1,%d", count);
		break;
	}
}