int callback_glewlwyd_get_scope (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_scope;
  
  j_scope = get_scope(config, u_map_get(request->map_url, "scope"));
  if (check_result_value(j_scope, G_OK)) {
    ulfius_set_json_body_response(response, 200, json_object_get(j_scope, "scope"));
  } else if (check_result_value(j_scope, G_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_get_scope - Error get_scope");
    response->status = 500;
  }
  json_decref(j_scope);
  return U_CALLBACK_CONTINUE;
}