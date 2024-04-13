_gsasl_gssapi_server_finish (Gsasl_session * sctx, void *mech_data)
{
  _Gsasl_gssapi_server_state *state = mech_data;
  OM_uint32 min_stat;

  if (!state)
    return;

  if (state->context != GSS_C_NO_CONTEXT)
    gss_delete_sec_context (&min_stat, &state->context, GSS_C_NO_BUFFER);

  if (state->cred != GSS_C_NO_CREDENTIAL)
    gss_release_cred (&min_stat, &state->cred);

  if (state->client != GSS_C_NO_NAME)
    gss_release_name (&min_stat, &state->client);

  free (state);
}