print_distance_range(FILE* f, OnigDistance a, OnigDistance b)
{
  if (a == ONIG_INFINITE_DISTANCE)
    fputs("inf", f);
  else
    fprintf(f, "(%"PRIuPTR")", a);

  fputs("-", f);

  if (b == ONIG_INFINITE_DISTANCE)
    fputs("inf", f);
  else
    fprintf(f, "(%"PRIuPTR")", b);
}