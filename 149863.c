extern int main(int argc ARG_UNUSED, char* argv[] ARG_UNUSED)
{
  int r;
  int i;
  FILE* fp = stdout;

  setlocale(LC_ALL, "C");
  /* setlocale(LC_ALL, "POSIX"); */
  /* setlocale(LC_ALL, "en_GB.iso88591"); */
  /* setlocale(LC_ALL, "de_BE.iso88591"); */
  /* setlocale(LC_ALL, "fr_FR.iso88591"); */

  for (i = 0; i < (int )(sizeof(Info)/sizeof(ENC_INFO)); i++) {
    r = exec(fp, &Info[i]);
    if (r < 0) {
      fprintf(stderr, "FAIL exec(): %d\n", r);
      return -1;
    }
  }

  return 0;
}