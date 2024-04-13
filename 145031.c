  bool escape(char * dest, const char * src, size_t limit, const char * others)
  {
    const char * begin = src;
    const char * end = dest + limit;
    if (asc_isspace(*src)) {
      if (dest == end) return false;
      *dest++ = '\\';
      if (dest == end) return false;
      *dest++ = *src++;
    }
    while (*src) {
      if (dest == end) return false;
      switch (*src) {
      case '\n': *dest++ = '\\'; *dest = 'n'; break;
      case '\r': *dest++ = '\\'; *dest = 'r'; break;
      case '\t': *dest++ = '\\'; *dest = 't'; break;
      case '\f': *dest++ = '\\'; *dest = 'f'; break;
      case '\v': *dest++ = '\\'; *dest = 'v'; break;
      case '\\': *dest++ = '\\'; *dest = '\\'; break;
      case '#' : *dest++ = '\\'; *dest = '#'; break;
      default:
	if (others && strchr(others, *src)) *dest++ = '\\';
	*dest = *src;
      }
      ++src;
      ++dest;
    }
    if (src > begin + 1 && asc_isspace(src[-1])) {
      --dest;
      *dest++ = '\\';
      if (dest == end) return false;
      *dest++ = src[-1];
    }
    *dest = '\0';
    return true;
  }