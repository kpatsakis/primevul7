_gsasl_gssapi_server_start (Gsasl_session * sctx, void **mech_data)
{
  _Gsasl_gssapi_server_state *state;

  state = (_Gsasl_gssapi_server_state *) malloc (sizeof (*state));
  if (state == NULL)
    return GSASL_MALLOC_ERROR;

  state->step = 0;
  state->cred = GSS_C_NO_CREDENTIAL;
  state->context = GSS_C_NO_CONTEXT;
  state->client = NULL;
  *mech_data = state;

  return GSASL_OK;
}