int callback_metrics (const struct _u_request * request, struct _u_response * response, void * user_data) {
  UNUSED(request);
  struct config_elements * config = (struct config_elements *)user_data;
  size_t i, j;
  char * content = o_strdup("# We have seen handsome noble-looking men but I have never seen a man like the one who now stands at the entrance of the gate.\n");
  struct _glwd_metric * metric;
  
  if (!pthread_mutex_lock(&config->metrics_lock)) {
    u_map_put(response->map_header, ULFIUS_HTTP_HEADER_CONTENT, "text/plain; charset=utf-8");
    for (i=0; i<pointer_list_size(&config->metrics_list); i++) {
      metric = (struct _glwd_metric *)pointer_list_get_at(&config->metrics_list, i);
      content = mstrcatf(content, "# HELP %s_total %s\n", metric->name, metric->help);
      content = mstrcatf(content, "# TYPE %s_total counter\n", metric->name);
      for (j=0; j<metric->data_size; j++) {
        if (metric->data[j].label != NULL) {
          content = mstrcatf(content, "%s_total{%s} %zu\n", metric->name, metric->data[j].label, metric->data[j].counter);
        } else {
          content = mstrcatf(content, "%s_total %zu\n", metric->name, metric->data[j].counter);
        }
      }
    }
    ulfius_set_string_body_response(response, 200, content);
    o_free(content);
    pthread_mutex_unlock(&config->metrics_lock);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_metrics - Error lock");
    response->status = 500;
  }
  return U_CALLBACK_CONTINUE;
}