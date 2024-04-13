int callback_glewlwyd_set_user (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_user, * j_user_valid, * j_search_user;
  
  j_search_user = get_user(config, u_map_get(request->map_url, "username"), u_map_get(request->map_url, "source"));
  if (check_result_value(j_search_user, G_OK)) {
    j_user = ulfius_get_json_body_request(request, NULL);
    if (j_user != NULL) {
      j_user_valid = is_user_valid(config, u_map_get(request->map_url, "username"), j_user, 0, json_string_value(json_object_get(json_object_get(j_search_user, "user"), "source")));
      if (check_result_value(j_user_valid, G_OK)) {
        if (set_user(config, u_map_get(request->map_url, "username"), j_user, json_string_value(json_object_get(json_object_get(j_search_user, "user"), "source"))) != G_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_set_user - Error set_user");
          response->status = 500;
        } else {
          y_log_message(Y_LOG_LEVEL_INFO, "Event - User '%s' updated", u_map_get(request->map_url, "username"));
        }
      } else if (check_result_value(j_user_valid, G_ERROR_PARAM)) {
        if (json_object_get(j_user_valid, "error") != NULL) {
          ulfius_set_json_body_response(response, 400, json_object_get(j_user_valid, "error"));
        } else {
          response->status = 400;
        }
      } else if (!check_result_value(j_user_valid, G_OK)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_set_user - Error is_user_valid");
        response->status = 500;
      }
      json_decref(j_user_valid);
    } else {
      response->status = 400;
    }
    json_decref(j_user);
  } else if (check_result_value(j_search_user, G_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_set_user - Error get_user");
    response->status = 500;
  }
  json_decref(j_search_user);
  return U_CALLBACK_CONTINUE;
}