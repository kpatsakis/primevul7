int callback_glewlwyd_user_delete_profile (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  int ret = G_OK;
  const char * username = json_string_value(json_object_get((json_t *)response->shared_data, "username"));
  json_t * j_session, * j_cur_session;
  char * session_uid = get_session_id(config, request);
  size_t index;

  j_session = get_current_user_for_session(config, session_uid);
  if (check_result_value(j_session, G_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else if (!check_result_value(j_session, G_OK)) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_delete_profile - Error get_current_user_for_session");
    response->status = 500;
  } else {
    json_array_foreach(json_object_get(j_session, "session"), index, j_cur_session) {
      if (0 == o_strcasecmp(username, json_string_value(json_object_get(j_cur_session, "username")))) {
        if (delete_user_session_from_hash(config, json_string_value(json_object_get(j_cur_session, "username")), NULL) != G_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_delete_profile - Error delete_user_session_from_hash");
          response->status = 500;
          ret = G_ERROR;
        } else {
          if (user_session_delete(config, session_uid, json_string_value(json_object_get(j_cur_session, "username"))) != G_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_delete_profile - Error user_session_delete");
            response->status = 500;
            ret = G_ERROR;
          } else {
            y_log_message(Y_LOG_LEVEL_INFO, "Event - User '%s' removed (profile)", json_string_value(json_object_get((json_t *)response->shared_data, "username")));
          }
        }
      }
    }
    json_decref(j_session);
    if (ret == G_OK) {
      ret = user_delete_profile(config, username);
      if (ret == G_ERROR_UNAUTHORIZED) {
        response->status = 403;
      } else if (ret != G_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_delete_profile - Error user_delete_profile");
        response->status = 500;
      }
    }
  }
  o_free(session_uid);
  
  return U_CALLBACK_CONTINUE;
}