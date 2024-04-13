failed(const char *s)
{
    perror(s);
    ExitProgram(EXIT_FAILURE);
}