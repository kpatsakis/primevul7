int callback_glewlwyd_user_get_plugin_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  UNUSED(request);
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_plugin_list = get_plugin_module_list_for_user(config);
  
  if (check_result_value(j_plugin_list, G_OK)) {
    ulfius_set_json_body_response(response, 200, json_object_get(j_plugin_list, "module"));
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_get_plugin_list - Error j_plugin_list");
    response->status = 500;
  }
  json_decref(j_plugin_list);
  return U_CALLBACK_CONTINUE;
}