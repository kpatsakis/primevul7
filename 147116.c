init_PRNG()
{
    char buffer[256];
    const char *file;
    long l;
    if (RAND_status())
	return;
    if ((file = RAND_file_name(buffer, sizeof(buffer)))) {
#ifdef USE_EGD
	if (RAND_egd(file) > 0)
	    return;
#endif
	RAND_load_file(file, -1);
    }
    if (RAND_status())
	goto seeded;
    srand48((long)time(NULL));
    while (!RAND_status()) {
	l = lrand48();
	RAND_seed((unsigned char *)&l, sizeof(long));
    }
  seeded:
    if (file)
	RAND_write_file(file);
}