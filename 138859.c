compare_routerstatus_entries(const void **_a, const void **_b)
{
  const routerstatus_t *a = *_a, *b = *_b;
  return fast_memcmp(a->identity_digest, b->identity_digest, DIGEST_LEN);
}