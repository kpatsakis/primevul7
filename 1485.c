int callback_glewlwyd_add_user (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_user, * j_user_valid, * j_search_user, * j_body;
  
  j_user = ulfius_get_json_body_request(request, NULL);
  if (j_user != NULL) {
    j_user_valid = is_user_valid(config, NULL, j_user, 1, u_map_get(request->map_url, "source"));
    if (check_result_value(j_user_valid, G_OK)) {
      j_search_user = get_user(config, json_string_value(json_object_get(j_user, "username")), u_map_get(request->map_url, "source"));
      if (check_result_value(j_search_user, G_ERROR_NOT_FOUND)) {
        if (add_user(config, j_user, u_map_get(request->map_url, "source")) != G_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_add_user - Error add_user");
          response->status = 500;
        } else {
          y_log_message(Y_LOG_LEVEL_INFO, "Event - User '%s' added", json_string_value(json_object_get(j_user, "username")));
        }
      } else if (check_result_value(j_search_user, G_OK)) {
        j_body = json_pack("{s[s]}", "error", "username already exists");
        ulfius_set_json_body_response(response, 400, j_body);
        json_decref(j_body);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_add_user - Error get_user");
        response->status = 500;
      }
      json_decref(j_search_user);
    } else if (check_result_value(j_user_valid, G_ERROR_PARAM)) {
      if (json_object_get(j_user_valid, "error") != NULL) {
        ulfius_set_json_body_response(response, 400, json_object_get(j_user_valid, "error"));
      } else {
        response->status = 400;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_add_user - Error is_user_valid");
      response->status = 500;
    }
    json_decref(j_user_valid);
  } else {
    response->status = 400;
  }
  json_decref(j_user);
  return U_CALLBACK_CONTINUE;
}