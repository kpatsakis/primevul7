int callback_glewlwyd_check_admin_session_delegate (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  char * session_uid;
  json_t * j_user, * j_delegate;
  int ret;
  
  if ((session_uid = get_session_id(config, request)) != NULL) {
    j_user = get_current_user_for_session(config, session_uid);
    if (check_result_value(j_user, G_OK) && json_object_get(json_object_get(j_user, "user"), "enabled") == json_true()) {
      if (is_scope_list_valid_for_session(config, config->admin_scope, session_uid) == G_OK) {
        j_delegate = get_user(config, u_map_get(request->map_url, "username"), NULL);
        if (check_result_value(j_delegate, G_OK)) {
          if (ulfius_set_response_shared_data(response, json_deep_copy(json_object_get(j_delegate, "user")), (void (*)(void *))&json_decref) != U_OK) {
            ret = U_CALLBACK_ERROR;
          } else {
            ret = U_CALLBACK_IGNORE;
          }
        } else {
          ret = U_CALLBACK_UNAUTHORIZED;
        }
        json_decref(j_delegate);
      } else {
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