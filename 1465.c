int callback_glewlwyd_set_user_module (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_module, * j_module_valid, * j_search_module;
  
  j_search_module = get_user_module(config, u_map_get(request->map_url, "name"));
  if (check_result_value(j_search_module, G_OK)) {
    j_module = ulfius_get_json_body_request(request, NULL);
    if (j_module != NULL) {
      json_object_del(j_module, "enabled");
      j_module_valid = is_user_module_valid(config, j_module, 0);
      if (check_result_value(j_module_valid, G_OK)) {
        if (set_user_module(config, u_map_get(request->map_url, "name"), j_module) != G_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_set_user_module - Error set_user_module");
          response->status = 500;
        } else {
          y_log_message(Y_LOG_LEVEL_INFO, "Event - User backend module '%s' updated", u_map_get(request->map_url, "name"));
        }
      } else if (check_result_value(j_module_valid, G_ERROR_PARAM)) {
        if (json_object_get(j_module_valid, "error") != NULL) {
          ulfius_set_json_body_response(response, 400, json_object_get(j_module_valid, "error"));
        } else {
          response->status = 400;
        }
      } else if (!check_result_value(j_module_valid, G_OK)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_set_user_module - Error is_user_module_valid");
        response->status = 500;
      }
      json_decref(j_module_valid);
    } else {
      response->status = 400;
    }
    json_decref(j_module);
  } else if (check_result_value(j_search_module, G_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_set_user_module - Error get_user_module");
    response->status = 500;
  }
  json_decref(j_search_module);
  return U_CALLBACK_CONTINUE;
}