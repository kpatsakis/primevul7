static int in_domain(const char *host, const char *domain)
{
  if (domain[0] == '.') {
    int l1 = strlen(host);
    int l2 = strlen(domain);
    if (l1 > l2) {
    	return strcmp(host+l1-l2,domain) == 0;
    } else {
      return 0;
    }
  } else {
    return strcmp(host,domain) == 0;
  }
}