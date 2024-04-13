static int gnutls_assert_val_int (int val, const char *file, int line)
{
  fprintf(stderr, "ASSERT: %s: %d\n", file, line);
  return val;
}