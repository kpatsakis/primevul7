int callback_glewlwyd_user_update_password (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_session, * j_password, * j_element = NULL;
  char * session_uid = get_session_id(config, request);
  const char ** passwords = NULL;
  int res;
  struct _user_module_instance * user_module;
  size_t index = 0;

  if (session_uid != NULL && o_strlen(session_uid)) {
    j_session = get_current_user_for_session(config, session_uid);
    if (check_result_value(j_session, G_OK)) {
      j_password = ulfius_get_json_body_request(request, NULL);
      user_module = get_user_module_instance(config, json_string_value(json_object_get(json_object_get(j_session, "user"), "source")));
      if (user_module && user_module->multiple_passwords) {
        if (json_string_length(json_object_get(j_password, "old_password")) && json_is_array(json_object_get(j_password, "password"))) {
          if ((passwords = o_malloc(json_array_size(json_object_get(j_password, "password")) * sizeof(char *))) != NULL) {
            json_array_foreach(json_object_get(j_password, "password"), index, j_element) {
              passwords[index] = json_string_value(j_element);
            }
            if ((res = user_update_password(config, json_string_value(json_object_get(json_object_get(j_session, "user"), "username")), json_string_value(json_object_get(j_password, "old_password")), passwords, json_array_size(json_object_get(j_password, "password")))) == G_ERROR_PARAM) {
              response->status = 400;
            } else if (res != G_OK) {
              y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_update_password - Error user_update_password (1)");
              response->status = 500;
            }
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_update_password - Error allocating resources for passwords (1)");
            response->status = 500;
          }
          o_free(passwords);
        } else {
          response->status = 400;
        }
      } else {
        if (json_string_length(json_object_get(j_password, "old_password")) && json_string_length(json_object_get(j_password, "password"))) {
          if ((passwords = o_malloc(sizeof(char *))) != NULL) {
            passwords[0] = json_string_value(json_object_get(j_password, "password"));
            if ((res = user_update_password(config, json_string_value(json_object_get(json_object_get(j_session, "user"), "username")), json_string_value(json_object_get(j_password, "old_password")), passwords, 1)) == G_ERROR_PARAM) {
              response->status = 400;
            } else if (res != G_OK) {
              y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_update_password - Error user_update_password (2)");
              response->status = 500;
            }
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_update_password - Error allocating resources for passwords (2)");
            response->status = 500;
          }
          o_free(passwords);
        } else {
          response->status = 400;
        }
      }
      json_decref(j_password);
    } else if (check_result_value(j_session, G_ERROR_NOT_FOUND)) {
      response->status = 401;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_update_password - Error get_current_user_for_session");
      response->status = 500;
    }
    json_decref(j_session);
  } else {
    response->status = 401;
  }
  o_free(session_uid);
  
  return U_CALLBACK_CONTINUE;
}