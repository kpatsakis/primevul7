void reds_update_client_mouse_allowed(RedsState *reds)
{
    int allow_now = FALSE;
    int x_res = 0;
    int y_res = 0;
    int num_active_workers = reds->qxl_instances.size();

    if (num_active_workers > 0) {

        allow_now = TRUE;
        FOREACH_QXL_INSTANCE(reds, qxl) {
            if (red_qxl_get_allow_client_mouse(qxl, &x_res, &y_res, &allow_now)) {
                break;
            }
        }
    }

    if (allow_now || allow_now != reds->dispatcher_allows_client_mouse) {
        reds->monitor_mode.x_res = x_res;
        reds->monitor_mode.y_res = y_res;
        reds->dispatcher_allows_client_mouse = allow_now;
        reds_update_mouse_mode(reds);
        if (reds->is_client_mouse_allowed && reds->inputs_channel &&
            reds->inputs_channel->has_tablet()) {
            reds->inputs_channel->set_tablet_logical_size(reds->monitor_mode.x_res, reds->monitor_mode.y_res);
        }
    }
}