sock_log(char *message, ...)
{
    FILE *f = fopen("zzzsocklog", "a");
    va_list va;

    if (f == NULL)
	return;
    va_start(va, message);
    vfprintf(f, message, va);
    fclose(f);
}