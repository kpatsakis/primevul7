int callback_glewlwyd_server_configuration (const struct _u_request * request, struct _u_response * response, void * user_data) {
  UNUSED(request);
  
  json_t * json_body = json_pack("{ssssssss}", 
                                 "api_prefix", 
                                 ((struct config_elements *)user_data)->api_prefix,
                                 "admin_scope",
                                 ((struct config_elements *)user_data)->admin_scope,
                                 "profile_scope",
                                 ((struct config_elements *)user_data)->profile_scope,
                                 "delete_profile",
                                 ((struct config_elements *)user_data)->delete_profile==GLEWLWYD_PROFILE_DELETE_UNAUTHORIZED?"no":"yes");
  ulfius_set_json_body_response(response, 200, json_body);
  json_decref(json_body);
  return U_CALLBACK_CONTINUE;
}