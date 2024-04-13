static void imap_set_flag (IMAP_DATA* idata, int aclbit, int flag,
                           const char *str, char *flags, size_t flsize)
{
  if (mutt_bit_isset (idata->ctx->rights, aclbit))
    if (flag && imap_has_flag (idata->flags, str))
      safe_strcat (flags, flsize, str);
}