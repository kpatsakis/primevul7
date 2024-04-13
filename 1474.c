int callback_glewlwyd_set_user_session_scope_grant (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_user = (json_t *)response->shared_data, * j_body = ulfius_get_json_body_request(request, NULL), * j_client;
  int res;
  
  if (config != NULL && j_user != NULL) {
    if (json_object_get(j_body, "scope") != NULL && json_is_string(json_object_get(j_body, "scope"))) {
      j_client = get_client(config, u_map_get(request->map_url, "client_id"), NULL);
      if (check_result_value(j_client, G_OK) && json_object_get(json_object_get(j_client, "client"), "enabled") == json_true()) {
        res = set_granted_scopes_for_client(config, j_user, u_map_get(request->map_url, "client_id"), json_string_value(json_object_get(j_body, "scope")));
        if (res == G_ERROR_NOT_FOUND) {
          response->status = 404;
        } else if (res == G_ERROR_PARAM) {
          response->status = 400;
        } else if (res == G_ERROR_UNAUTHORIZED) {
          response->status = 401;
        } else if (res != G_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_set_user_session_scope_grant - Error set_granted_scopes_for_client");
          response->status = 500;
        } else {
          y_log_message(Y_LOG_LEVEL_INFO, "Event - User '%s' granted scope list '%s' for client '%s'", json_string_value(json_object_get(j_user, "username")), json_string_value(json_object_get(j_body, "scope")), u_map_get(request->map_url, "client_id"));
        }
      } else if (check_result_value(j_client, G_ERROR_NOT_FOUND) || json_object_get(json_object_get(j_client, "client"), "enabled") != json_true()) {
        response->status = 404;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_set_user_session_scope_grant - Error get_client");
        response->status = 500;
      }
      json_decref(j_client);
    } else {
      response->status = 400;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_set_user_session_scope_grant - Error config or j_user is NULL");
    response->status = 500;
  }
  json_decref(j_body);
  return U_CALLBACK_CONTINUE;
}