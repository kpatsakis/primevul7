int callback_glewlwyd_delete_client_module (const struct _u_request * request, struct _u_response * response, void * client_data) {
  struct config_elements * config = (struct config_elements *)client_data;
  json_t * j_search_module;
  
  j_search_module = get_client_module(config, u_map_get(request->map_url, "name"));
  if (check_result_value(j_search_module, G_OK)) {
    if (delete_client_module(config, u_map_get(request->map_url, "name")) != G_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_delete_client_module - Error delete_client_module");
      response->status = 500;
    } else {
      y_log_message(Y_LOG_LEVEL_INFO, "Event - Client backend module '%s' removed", u_map_get(request->map_url, "name"));
    }
  } else if (check_result_value(j_search_module, G_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_delete_client_module - Error get_client_module");
    response->status = 500;
  }
  json_decref(j_search_module);
  return U_CALLBACK_CONTINUE;
}