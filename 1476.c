int callback_glewlwyd_add_client_module (const struct _u_request * request, struct _u_response * response, void * client_data) {
  struct config_elements * config = (struct config_elements *)client_data;
  json_t * j_module, * j_module_valid, * j_result;
  
  j_module = ulfius_get_json_body_request(request, NULL);
  if (j_module != NULL) {
    j_module_valid = is_client_module_valid(config, j_module, 1);
    if (check_result_value(j_module_valid, G_OK)) {
      j_result = add_client_module(config, j_module);
      if (check_result_value(j_result, G_ERROR_PARAM)) {
        if (json_object_get(j_result, "error") != NULL) {
          ulfius_set_json_body_response(response, 400, json_object_get(j_result, "error"));
        } else {
          response->status = 400;
        }
      } else if (!check_result_value(j_result, G_OK)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_add_client_module - Error add_client_module");
        response->status = 500;
      } else {
        y_log_message(Y_LOG_LEVEL_INFO, "Event - Client backend module '%s' added (%s)", json_string_value(json_object_get(j_module, "name")), json_string_value(json_object_get(j_module, "module")));
      }
      json_decref(j_result);
    } else if (check_result_value(j_module_valid, G_ERROR_PARAM)) {
      if (json_object_get(j_module_valid, "error") != NULL) {
        ulfius_set_json_body_response(response, 400, json_object_get(j_module_valid, "error"));
      } else {
        response->status = 400;
      }
    } else if (!check_result_value(j_module_valid, G_OK)) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_add_client_module - Error is_client_module_valid");
      response->status = 500;
    }
    json_decref(j_module_valid);
  } else {
    response->status = 400;
  }
  json_decref(j_module);
  return U_CALLBACK_CONTINUE;
}