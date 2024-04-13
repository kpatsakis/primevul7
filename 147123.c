write_from_file(int sock, char *file)
{
    FILE *fd;
    int c;
    char buf[1];
    fd = fopen(file, "r");
    if (fd != NULL) {
	while ((c = fgetc(fd)) != EOF) {
	    buf[0] = c;
	    write(sock, buf, 1);
	}
	fclose(fd);
    }
}