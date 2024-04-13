int callback_glewlwyd_add_scope (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_scope, * j_scope_valid, * j_search_scope, * j_body;
  
  j_scope = ulfius_get_json_body_request(request, NULL);
  if (j_scope != NULL) {
    j_scope_valid = is_scope_valid(config, j_scope, 1);
    if (check_result_value(j_scope_valid, G_OK)) {
      j_search_scope = get_scope(config, json_string_value(json_object_get(j_scope, "name")));
      if (check_result_value(j_search_scope, G_ERROR_NOT_FOUND)) {
        if (add_scope(config, j_scope) != G_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_add_scope - Error add_scope");
          response->status = 500;
        } else {
          y_log_message(Y_LOG_LEVEL_INFO, "Event - Scope '%s' added", json_string_value(json_object_get(j_scope, "name")));
        }
      } else if (check_result_value(j_search_scope, G_OK)) {
        j_body = json_pack("{s[s]}", "error", "scope already exists");
        ulfius_set_json_body_response(response, 400, j_body);
        json_decref(j_body);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_add_scope - Error get_scope");
        response->status = 500;
      }
      json_decref(j_search_scope);
    } else if (check_result_value(j_scope_valid, G_ERROR_PARAM)) {
      ulfius_set_json_body_response(response, 400, json_object_get(j_scope_valid, "error"));
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_add_scope - Error is_scope_valid");
      response->status = 500;
    }
    json_decref(j_scope_valid);
  } else {
    response->status = 400;
  }
  json_decref(j_scope);
  return U_CALLBACK_CONTINUE;
}