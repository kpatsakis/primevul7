static CURLcode set_login(struct connectdata *conn,
                          const char *user, const char *passwd,
                          const char *options)
{
  CURLcode result = CURLE_OK;

  /* If our protocol needs a password and we have none, use the defaults */
  if((conn->handler->flags & PROTOPT_NEEDSPWD) && !conn->bits.user_passwd) {
    /* Store the default user */
    conn->user = strdup(CURL_DEFAULT_USER);

    /* Store the default password */
    if(conn->user)
      conn->passwd = strdup(CURL_DEFAULT_PASSWORD);
    else
      conn->passwd = NULL;

    /* This is the default password, so DON'T set conn->bits.user_passwd */
  }
  else {
    /* Store the user, zero-length if not set */
    conn->user = strdup(user);

    /* Store the password (only if user is present), zero-length if not set */
    if(conn->user)
      conn->passwd = strdup(passwd);
    else
      conn->passwd = NULL;
  }

  if(!conn->user || !conn->passwd)
    result = CURLE_OUT_OF_MEMORY;

  /* Store the options, null if not set */
  if(!result && options[0]) {
    conn->options = strdup(options);

    if(!conn->options)
      result = CURLE_OUT_OF_MEMORY;
  }

  return result;
}