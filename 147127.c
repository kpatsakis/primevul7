SSL_write_from_file(SSL * ssl, char *file)
{
    FILE *fd;
    int c;
    char buf[1];
    fd = fopen(file, "r");
    if (fd != NULL) {
	while ((c = fgetc(fd)) != EOF) {
	    buf[0] = c;
	    SSL_write(ssl, buf, 1);
	}
	fclose(fd);
    }
}