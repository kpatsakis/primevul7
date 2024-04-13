int callback_glewlwyd_set_client (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_client, * j_client_valid, * j_search_client;
  
  j_search_client = get_client(config, u_map_get(request->map_url, "client_id"), u_map_get(request->map_url, "source"));
  if (check_result_value(j_search_client, G_OK)) {
    j_client = ulfius_get_json_body_request(request, NULL);
    if (j_client != NULL) {
      j_client_valid = is_client_valid(config, u_map_get(request->map_url, "client_id"), j_client, 0, json_string_value(json_object_get(json_object_get(j_search_client, "client"), "source")));
      if (check_result_value(j_client_valid, G_OK)) {
        if (set_client(config, u_map_get(request->map_url, "client_id"), j_client, json_string_value(json_object_get(json_object_get(j_search_client, "client"), "source"))) != G_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_set_client - Error set_client");
          response->status = 500;
        } else {
          y_log_message(Y_LOG_LEVEL_INFO, "Event - Client '%s' updated", u_map_get(request->map_url, "client_id"));
        }
      } else if (check_result_value(j_client_valid, G_ERROR_PARAM)) {
        if (json_object_get(j_client_valid, "error") != NULL) {
          ulfius_set_json_body_response(response, 400, json_object_get(j_client_valid, "error"));
        } else {
          response->status = 400;
        }
      } else if (!check_result_value(j_client_valid, G_OK)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_set_client - Error is_client_valid");
        response->status = 500;
      }
      json_decref(j_client_valid);
    } else {
      response->status = 400;
    }
    json_decref(j_client);
  } else if (check_result_value(j_search_client, G_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_set_client - Error get_client");
    response->status = 500;
  }
  json_decref(j_search_client);
  return U_CALLBACK_CONTINUE;
}