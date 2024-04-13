int callback_glewlwyd_add_api_key (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  const char * issued_for = get_ip_source(request), * username = json_string_value(json_object_get((json_t *)response->shared_data, "username")), * user_agent = u_map_get_case(request->map_header, "user-agent");
  json_t * j_api_key = generate_api_key(config, username, issued_for, user_agent);
  
  if (check_result_value(j_api_key, G_OK)) {
    ulfius_set_json_body_response(response, 200, json_object_get(j_api_key, "api_key"));
    y_log_message(Y_LOG_LEVEL_INFO, "Event - API key created for user '%s'", username);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_add_api_key - Error generate_api_key");
    response->status = 500;
  }
  json_decref(j_api_key);
  return U_CALLBACK_CONTINUE;
}