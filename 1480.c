int callback_glewlwyd_delete_session (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  int res = delete_user_session_from_hash(config, json_string_value(json_object_get((json_t *)response->shared_data, "username")), u_map_get(request->map_url, "session_hash"));
  if (res == G_ERROR_NOT_FOUND) {
    response->status = 404;
  } else if (res != G_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_delete_session - Error delete_user_session_from_hash");
    response->status = 500;
  }
  return U_CALLBACK_CONTINUE;
}