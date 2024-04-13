onig_snprintf_with_pattern(UChar buf[], int bufsize, OnigEncoding enc,
                           UChar* pat, UChar* pat_end, const UChar *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  onig_vsnprintf_with_pattern(buf, bufsize, enc,
	  pat, pat_end, fmt, args);
  va_end(args);
}