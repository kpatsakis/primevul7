int callback_glewlwyd_check_user_profile_valid (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  char * session_uid;
  json_t * j_user;
  int ret, res;
  
  if ((session_uid = get_session_id(config, request)) != NULL) {
    j_user = get_current_user_for_session(config, session_uid);
    if (check_result_value(j_user, G_OK) && json_object_get(json_object_get(j_user, "user"), "enabled") == json_true()) {
      if ((res = is_scope_list_valid_for_session(config, config->profile_scope, session_uid)) == G_OK) {
        if (ulfius_set_response_shared_data(response, json_deep_copy(json_object_get(j_user, "user")), (void (*)(void *))&json_decref) != U_OK) {
          ret = U_CALLBACK_ERROR;
        } else {
          ret = U_CALLBACK_IGNORE;
        }
      } else {
        if (res == G_ERROR) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_check_user_session - Error is_scope_list_valid_for_session");
        }
        ret = U_CALLBACK_UNAUTHORIZED;
      }
    } else {
      ret = U_CALLBACK_UNAUTHORIZED;
    }
    json_decref(j_user);
  } else {
    ret = U_CALLBACK_UNAUTHORIZED;
  }
  o_free(session_uid);
  return ret;
}