save_page_device(gs_gstate *pgs)
{
    return
        (r_has_type(&gs_int_gstate(pgs)->pagedevice, t_null) &&
         (*dev_proc(gs_currentdevice(pgs), get_page_device))(gs_currentdevice(pgs)) != 0);
}