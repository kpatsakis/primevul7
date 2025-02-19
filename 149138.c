_gsasl_gssapi_server_step (Gsasl_session * sctx,
			   void *mech_data,
			   const char *input, size_t input_len,
			   char **output, size_t *output_len)
{
  _Gsasl_gssapi_server_state *state = mech_data;
  gss_buffer_desc bufdesc1, bufdesc2;
  OM_uint32 maj_stat, min_stat;
  gss_buffer_desc client_name;
  gss_OID mech_type;
  char tmp[4];
  int res;

  *output = NULL;
  *output_len = 0;

  switch (state->step)
    {
    case 0:
      {
	gss_name_t server;
	const char *service;
	const char *hostname;

	if (input_len == 0)
	  {
	    res = GSASL_NEEDS_MORE;
	    break;
	  }

	service = gsasl_property_get (sctx, GSASL_SERVICE);
	if (!service)
	  return GSASL_NO_SERVICE;

	hostname = gsasl_property_get (sctx, GSASL_HOSTNAME);
	if (!hostname)
	  return GSASL_NO_HOSTNAME;

	/* FIXME: Use asprintf. */

	bufdesc1.length = strlen (service) + strlen ("@")
	  + strlen (hostname) + 1;
	bufdesc1.value = malloc (bufdesc1.length);
	if (bufdesc1.value == NULL)
	  return GSASL_MALLOC_ERROR;

	sprintf (bufdesc1.value, "%s@%s", service, hostname);

	maj_stat = gss_import_name (&min_stat, &bufdesc1,
				    GSS_C_NT_HOSTBASED_SERVICE, &server);
	free (bufdesc1.value);
	if (GSS_ERROR (maj_stat))
	  return GSASL_GSSAPI_IMPORT_NAME_ERROR;

	maj_stat = gss_acquire_cred (&min_stat, server, 0,
				     GSS_C_NULL_OID_SET, GSS_C_ACCEPT,
				     &state->cred, NULL, NULL);
	gss_release_name (&min_stat, &server);
	if (GSS_ERROR (maj_stat))
	  return GSASL_GSSAPI_ACQUIRE_CRED_ERROR;
      }
      state->step++;
      /* fall through */

    case 1:
      bufdesc1.value = (void *) input;
      bufdesc1.length = input_len;
      if (state->client)
	{
	  gss_release_name (&min_stat, &state->client);
	  state->client = GSS_C_NO_NAME;
	}

      maj_stat = gss_accept_sec_context (&min_stat,
					 &state->context,
					 state->cred,
					 &bufdesc1,
					 GSS_C_NO_CHANNEL_BINDINGS,
					 &state->client,
					 &mech_type,
					 &bufdesc2, NULL, NULL, NULL);
      if (maj_stat != GSS_S_COMPLETE && maj_stat != GSS_S_CONTINUE_NEEDED)
	return GSASL_GSSAPI_ACCEPT_SEC_CONTEXT_ERROR;

      if (maj_stat == GSS_S_COMPLETE)
	state->step++;

      if (maj_stat == GSS_S_CONTINUE_NEEDED || bufdesc2.length > 0)
	{
	  *output = malloc (bufdesc2.length);
	  if (!*output)
	    return GSASL_MALLOC_ERROR;
	  memcpy (*output, bufdesc2.value, bufdesc2.length);
	  *output_len = bufdesc2.length;
	}

      maj_stat = gss_release_buffer (&min_stat, &bufdesc2);
      if (GSS_ERROR (maj_stat))
	return GSASL_GSSAPI_RELEASE_BUFFER_ERROR;

      if (maj_stat == GSS_S_CONTINUE_NEEDED || *output_len > 0)
	{
	  res = GSASL_NEEDS_MORE;
	  break;
	}
      /* fall through */

    case 2:
      memset (tmp, 0xFF, 4);
      tmp[0] = GSASL_QOP_AUTH;
      bufdesc1.length = 4;
      bufdesc1.value = tmp;
      maj_stat = gss_wrap (&min_stat, state->context, 0, GSS_C_QOP_DEFAULT,
			   &bufdesc1, NULL, &bufdesc2);
      if (GSS_ERROR (maj_stat))
	return GSASL_GSSAPI_WRAP_ERROR;

      *output = malloc (bufdesc2.length);
      if (!*output)
	return GSASL_MALLOC_ERROR;
      memcpy (*output, bufdesc2.value, bufdesc2.length);
      *output_len = bufdesc2.length;

      maj_stat = gss_release_buffer (&min_stat, &bufdesc2);
      if (GSS_ERROR (maj_stat))
	return GSASL_GSSAPI_RELEASE_BUFFER_ERROR;

      state->step++;
      res = GSASL_NEEDS_MORE;
      break;

    case 3:
      bufdesc1.value = (void *) input;
      bufdesc1.length = input_len;
      maj_stat = gss_unwrap (&min_stat, state->context, &bufdesc1,
			     &bufdesc2, NULL, NULL);
      if (GSS_ERROR (maj_stat))
	return GSASL_GSSAPI_UNWRAP_ERROR;

      /* [RFC 2222 section 7.2.1]:
         The client passes this token to GSS_Unwrap and interprets the
         first octet of resulting cleartext as a bit-mask specifying
         the security layers supported by the server and the second
         through fourth octets as the maximum size output_message to
         send to the server.  The client then constructs data, with
         the first octet containing the bit-mask specifying the
         selected security layer, the second through fourth octets
         containing in network byte order the maximum size
         output_message the client is able to receive, and the
         remaining octets containing the authorization identity.  The
         client passes the data to GSS_Wrap with conf_flag set to
         FALSE, and responds with the generated output_message.  The
         client can then consider the server authenticated. */

      if (bufdesc2.length < 4)
	return GSASL_AUTHENTICATION_ERROR;

      if ((((char *) bufdesc2.value)[0] & GSASL_QOP_AUTH) == 0)
	{
	  /* Integrity or privacy unsupported */
	  maj_stat = gss_release_buffer (&min_stat, &bufdesc2);
	  return GSASL_GSSAPI_UNSUPPORTED_PROTECTION_ERROR;
	}

      gsasl_property_set_raw (sctx, GSASL_AUTHZID,
			      (char *) bufdesc2.value + 4,
			      bufdesc2.length - 4);

      maj_stat = gss_display_name (&min_stat, state->client,
				   &client_name, &mech_type);
      if (GSS_ERROR (maj_stat))
	return GSASL_GSSAPI_DISPLAY_NAME_ERROR;

      gsasl_property_set_raw (sctx, GSASL_GSSAPI_DISPLAY_NAME,
			      client_name.value, client_name.length);

      maj_stat = gss_release_buffer (&min_stat, &client_name);
      if (GSS_ERROR (maj_stat))
	return GSASL_GSSAPI_RELEASE_BUFFER_ERROR;

      maj_stat = gss_release_buffer (&min_stat, &bufdesc2);
      if (GSS_ERROR (maj_stat))
	return GSASL_GSSAPI_RELEASE_BUFFER_ERROR;

      res = gsasl_callback (NULL, sctx, GSASL_VALIDATE_GSSAPI);

      state->step++;
      break;

    default:
      res = GSASL_MECHANISM_CALLED_TOO_MANY_TIMES;
      break;
    }

  return res;
}