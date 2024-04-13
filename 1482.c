int callback_glewlwyd_reload_modules (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct config_elements * config = (struct config_elements *)user_data;
  UNUSED(request);

  close_user_module_instance_list(config);
  close_user_module_list(config);
  
  close_user_middleware_module_instance_list(config);
  close_user_middleware_module_list(config);
  
  close_client_module_instance_list(config);
  close_client_module_list(config);
  
  close_user_auth_scheme_module_instance_list(config);
  close_user_auth_scheme_module_list(config);
  
  close_plugin_module_instance_list(config);
  close_plugin_module_list(config);
  
  // Initialize user modules
  if (init_user_module_list(config) != G_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error initializing user modules");
    response->status = 500;
  }
  if (load_user_module_instance_list(config) != G_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error loading user modules instances");
    response->status = 500;
  }

  // Initialize user modules
  if (init_user_middleware_module_list(config) != G_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error initializing user middleware modules");
    response->status = 500;
  }
  if (load_user_middleware_module_instance_list(config) != G_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error loading user middleware modules instances");
    response->status = 500;
  }

  // Initialize client modules
  if (init_client_module_list(config) != G_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error initializing client modules");
    response->status = 500;
  }
  if (load_client_module_instance_list(config) != G_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error loading client modules instances");
    response->status = 500;
  }

  // Initialize user auth scheme modules
  if (init_user_auth_scheme_module_list(config) != G_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error initializing user auth scheme modules");
    response->status = 500;
  }
  if (load_user_auth_scheme_module_instance_list(config) != G_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error loading user auth scheme modules instances");
    response->status = 500;
  }

  // Initialize plugins
  if (init_plugin_module_list(config) != G_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error initializing plugins modules");
    response->status = 500;
  }
  if (load_plugin_module_instance_list(config) != G_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error loading plugins modules instances");
    response->status = 500;
  }

  return U_CALLBACK_CONTINUE;
}