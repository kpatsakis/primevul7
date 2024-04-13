static int imap_check_capabilities (IMAP_DATA* idata)
{
  if (imap_exec (idata, "CAPABILITY", 0) != 0)
  {
    imap_error ("imap_check_capabilities", idata->buf);
    return -1;
  }

  if (!(mutt_bit_isset(idata->capabilities,IMAP4) ||
        mutt_bit_isset(idata->capabilities,IMAP4REV1)))
  {
    mutt_error _("This IMAP server is ancient. Mutt does not work with it.");
    mutt_sleep (2);	/* pause a moment to let the user see the error */

    return -1;
  }

  return 0;
}