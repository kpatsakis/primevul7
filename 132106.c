auth_spa_client(
  auth_instance *ablock,                 /* authenticator block */
  void * sx,				 /* connection */
  int timeout,                           /* command timeout */
  uschar *buffer,                        /* buffer for reading response */
  int buffsize)                          /* size of buffer */
{
auth_spa_options_block *ob =
       (auth_spa_options_block *)(ablock->options_block);
SPAAuthRequest   request;
SPAAuthChallenge challenge;
SPAAuthResponse  response;
char msgbuf[2048];
char *domain = NULL;
char *username, *password;

/* Code added by PH to expand the options */

*buffer = 0;    /* Default no message when cancelled */

if (!(username = CS expand_string(ob->spa_username)))
  {
  if (f.expand_string_forcedfail) return CANCELLED;
  string_format(buffer, buffsize, "expansion of \"%s\" failed in %s "
   "authenticator: %s", ob->spa_username, ablock->name,
   expand_string_message);
  return ERROR;
  }

if (!(password = CS expand_string(ob->spa_password)))
  {
  if (f.expand_string_forcedfail) return CANCELLED;
  string_format(buffer, buffsize, "expansion of \"%s\" failed in %s "
   "authenticator: %s", ob->spa_password, ablock->name,
   expand_string_message);
  return ERROR;
  }

if (ob->spa_domain)
  if (!(domain = CS expand_string(ob->spa_domain)))
    {
    if (f.expand_string_forcedfail) return CANCELLED;
    string_format(buffer, buffsize, "expansion of \"%s\" failed in %s "
		  "authenticator: %s", ob->spa_domain, ablock->name,
		  expand_string_message);
    return ERROR;
    }

/* Original code */

if (smtp_write_command(sx, SCMD_FLUSH, "AUTH %s\r\n", ablock->public_name) < 0)
  return FAIL_SEND;

/* wait for the 3XX OK message */
if (!smtp_read_response(sx, US buffer, buffsize, '3', timeout))
  return FAIL;

DSPA("\n\n%s authenticator: using domain %s\n\n", ablock->name, domain);

spa_build_auth_request(&request, CS username, domain);
spa_bits_to_base64(US msgbuf, US &request, spa_request_length(&request));

DSPA("\n\n%s authenticator: sending request (%s)\n\n", ablock->name, msgbuf);

/* send the encrypted password */
if (smtp_write_command(sx, SCMD_FLUSH, "%s\r\n", msgbuf) < 0)
  return FAIL_SEND;

/* wait for the auth challenge */
if (!smtp_read_response(sx, US buffer, buffsize, '3', timeout))
  return FAIL;

/* convert the challenge into the challenge struct */
DSPA("\n\n%s authenticator: challenge (%s)\n\n", ablock->name, buffer + 4);
spa_base64_to_bits(CS (&challenge), sizeof(challenge), CCS (buffer + 4));

spa_build_auth_response(&challenge, &response, CS username, CS password);
spa_bits_to_base64(US msgbuf, US &response, spa_request_length(&response));
DSPA("\n\n%s authenticator: challenge response (%s)\n\n", ablock->name, msgbuf);

/* send the challenge response */
if (smtp_write_command(sx, SCMD_FLUSH, "%s\r\n", msgbuf) < 0)
       return FAIL_SEND;

/* If we receive a success response from the server, authentication
has succeeded. There may be more data to send, but is there any point
in provoking an error here? */

if (smtp_read_response(sx, US buffer, buffsize, '2', timeout))
  return OK;

/* Not a success response. If errno != 0 there is some kind of transmission
error. Otherwise, check the response code in the buffer. If it starts with
'3', more data is expected. */

if (errno != 0 || buffer[0] != '3')
  return FAIL;

return FAIL;
}