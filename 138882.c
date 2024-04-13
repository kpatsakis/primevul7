token_clear(directory_token_t *tok)
{
  if (tok->key)
    crypto_pk_free(tok->key);
}