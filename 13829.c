static void reds_set_mouse_mode(RedsState *reds, SpiceMouseMode mode)
{
    if (reds->mouse_mode == mode) {
        return;
    }
    reds->mouse_mode = mode;

    FOREACH_QXL_INSTANCE(reds, qxl) {
        red_qxl_set_mouse_mode(qxl, mode);
    }

    reds->main_channel->push_mouse_mode(reds->mouse_mode,
                                        reds->is_client_mouse_allowed);
}