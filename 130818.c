get_quote_count(const char *line)
{
  int quote_count= 0;
  const char *quote= line;

  while ((quote= strpbrk(quote, "'`\"")) != NULL) {
    quote_count++;
    quote++;
  }

  return quote_count;
}