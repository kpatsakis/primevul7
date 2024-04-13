AP_DECLARE(int) ap_if_walk(request_rec *r)
{
    core_dir_config *dconf = ap_get_core_module_config(r->per_dir_config);
    int status = ap_if_walk_sub(r, dconf);
    return status;
}