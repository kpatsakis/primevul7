int callback_glewlwyd_check_admin_session_or_api_key (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  char * session_uid = NULL;
  json_t * j_user;
  int ret, res;
  const char * api_key = u_map_get_case(request->map_header, GLEWLWYD_API_KEY_HEADER_KEY), * ip_source = get_ip_source(request);
  
  if (NULL != api_key && 0 == o_strncmp(GLEWLWYD_API_KEY_HEADER_PREFIX, api_key, o_strlen(GLEWLWYD_API_KEY_HEADER_PREFIX))) {
    if ((res = verify_api_key(config, api_key + o_strlen(GLEWLWYD_API_KEY_HEADER_PREFIX))) == G_OK) {
      if (ulfius_set_response_shared_data(response, json_pack("{so}", "username", json_null()), (void (*)(void *))&json_decref) != U_OK) {
        ret = U_CALLBACK_ERROR;
      } else {
        ret = U_CALLBACK_IGNORE;
      }
    } else if (res == G_ERROR_UNAUTHORIZED) {
      y_log_message(Y_LOG_LEVEL_WARNING, "Security - API key invalid at IP Address %s", ip_source);
      ret = U_CALLBACK_UNAUTHORIZED;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_check_admin_session_or_api_key - Error verify_api_key");
      ret = U_CALLBACK_ERROR;
    }
  } else if ((session_uid = get_session_id(config, request)) != NULL) {
    j_user = get_current_user_for_session(config, session_uid);
    if (check_result_value(j_user, G_OK) && json_object_get(json_object_get(j_user, "user"), "enabled") == json_true()) {
      if ((res = is_scope_list_valid_for_session(config, config->admin_scope, session_uid)) == G_OK) {
        if (ulfius_set_response_shared_data(response, json_deep_copy(json_object_get(j_user, "user")), (void (*)(void *))&json_decref) != U_OK) {
          ret = U_CALLBACK_ERROR;
        } else {
          ret = U_CALLBACK_IGNORE;
        }
      } else {
        if (res == G_ERROR) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_check_admin_session_or_api_key - Error is_scope_list_valid_for_session");
        }
        ret = U_CALLBACK_UNAUTHORIZED;
      }
    } else {
      ret = U_CALLBACK_UNAUTHORIZED;
    }
    json_decref(j_user);
    o_free(session_uid);
  } else {
    ret = U_CALLBACK_UNAUTHORIZED;
  }
  return ret;
}