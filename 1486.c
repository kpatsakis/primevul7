int callback_glewlwyd_user_get_scheme_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  UNUSED(request);
  struct config_elements * config = (struct config_elements *)user_data;
  json_t * j_scheme_list = get_scheme_list_for_user(config, json_string_value(json_object_get((json_t *)response->shared_data, "username"))), * j_element;
  size_t index;
  
  if (check_result_value(j_scheme_list, G_OK)) {
    json_array_foreach(json_object_get(j_scheme_list, "scheme"), index, j_element) {
      json_object_del(j_element, "parameters");
    }
    ulfius_set_json_body_response(response, 200, json_object_get(j_scheme_list, "scheme"));
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_glewlwyd_user_get_scheme_list - Error get_scheme_list_for_user");
    response->status = 500;
  }
  json_decref(j_scheme_list);
  return U_CALLBACK_CONTINUE;
}