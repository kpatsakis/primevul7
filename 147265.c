int unit_load(Unit *u) {
        int r;

        assert(u);

        if (u->in_load_queue) {
                LIST_REMOVE(load_queue, u->manager->load_queue, u);
                u->in_load_queue = false;
        }

        if (u->type == _UNIT_TYPE_INVALID)
                return -EINVAL;

        if (u->load_state != UNIT_STUB)
                return 0;

        if (UNIT_VTABLE(u)->load) {
                r = UNIT_VTABLE(u)->load(u);
                if (r < 0)
                        goto fail;
        }

        if (u->load_state == UNIT_STUB) {
                r = -ENOENT;
                goto fail;
        }

        if (u->load_state == UNIT_LOADED) {

                if (u->default_dependencies) {
                        r = unit_add_default_dependencies(u);
                        if (r < 0)
                                goto fail;
                }

                unit_update_member_masks(u);

                r = unit_add_mount_links(u);
                if (r < 0)
                        goto fail;

                if (u->on_failure_job_mode == JOB_ISOLATE &&
                    set_size(u->dependencies[UNIT_ON_FAILURE]) > 1) {

                        log_error_unit(u->id,
                                       "More than one OnFailure= dependencies specified for %s but OnFailureJobMode=isolate set. Refusing.", u->id);

                        r = -EINVAL;
                        goto fail;
                }
        }

        assert((u->load_state != UNIT_MERGED) == !u->merged_into);

        unit_add_to_dbus_queue(unit_follow_merge(u));
        unit_add_to_gc_queue(u);

        return 0;

fail:
        u->load_state = u->load_state == UNIT_STUB ? UNIT_NOT_FOUND : UNIT_ERROR;
        u->load_error = r;
        unit_add_to_dbus_queue(u);
        unit_add_to_gc_queue(u);

        log_debug_unit(u->id, "Failed to load configuration for %s: %s",
                       u->id, strerror(-r));

        return r;
}