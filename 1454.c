int callback_glewlwyd_user_update_profile (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_profile, * j_result;

  j_profile = ulfius_get_json_body_request(request, NULL);
  if (j_profile != NULL && json_is_object(j_profile)) {
    j_result = user_set_profile(config, json_string_value(json_object_get((json_t *)response->shared_data, "username")), j_profile);
    if (check_result_value(j_result, G_ERROR_PARAM)) {
      if (json_object_get(j_result, "error") != NULL) {
        ulfius_set_json_body_response(response, 400, json_object_get(j_result, "error"));
      } else {
        response->status = 400;
      }
    } else if (!check_result_value(j_result, G_OK)) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_update_profile - Error user_set_profile");
      response->status = 500;
    } else {
      y_log_message(Y_LOG_LEVEL_INFO, "Event - User '%s' updated (profile)", json_string_value(json_object_get((json_t *)response->shared_data, "username")));
    }
    json_decref(j_result);
  } else {
    response->status = 400;
  }
  json_decref(j_profile);
  
  return U_CALLBACK_CONTINUE;
}