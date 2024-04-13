tor_version_compare(tor_version_t *a, tor_version_t *b)
{
  int i;
  tor_assert(a);
  tor_assert(b);

  /* We take this approach to comparison to ensure the same (bogus!) behavior
   * on all inputs as we would have seen before bug #21278 was fixed. The
   * only important difference here is that this method doesn't cause
   * a signed integer underflow.
   */
#define CMP(field) do {                               \
    unsigned aval = (unsigned) a->field;              \
    unsigned bval = (unsigned) b->field;              \
    int result = (int) (aval - bval);                 \
    if (result < 0)                                   \
      return -1;                                      \
    else if (result > 0)                              \
      return 1;                                       \
  } while (0)

  CMP(major);
  CMP(minor);
  CMP(micro);
  CMP(status);
  CMP(patchlevel);
  if ((i = strcmp(a->status_tag, b->status_tag)))
     return i;
  CMP(svn_revision);
  CMP(git_tag_len);
  if (a->git_tag_len)
     return fast_memcmp(a->git_tag, b->git_tag, a->git_tag_len);
  else
     return 0;

#undef CMP
}