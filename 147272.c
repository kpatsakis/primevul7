void unit_free(Unit *u) {
        UnitDependency d;
        Iterator i;
        char *t;

        assert(u);

        if (u->manager->n_reloading <= 0)
                unit_remove_transient(u);

        bus_unit_send_removed_signal(u);

        if (u->load_state != UNIT_STUB)
                if (UNIT_VTABLE(u)->done)
                        UNIT_VTABLE(u)->done(u);

        unit_free_requires_mounts_for(u);

        SET_FOREACH(t, u->names, i)
                hashmap_remove_value(u->manager->units, t, u);

        if (u->job) {
                Job *j = u->job;
                job_uninstall(j);
                job_free(j);
        }

        if (u->nop_job) {
                Job *j = u->nop_job;
                job_uninstall(j);
                job_free(j);
        }

        for (d = 0; d < _UNIT_DEPENDENCY_MAX; d++)
                bidi_set_free(u, u->dependencies[d]);

        if (u->type != _UNIT_TYPE_INVALID)
                LIST_REMOVE(units_by_type, u->manager->units_by_type[u->type], u);

        if (u->in_load_queue)
                LIST_REMOVE(load_queue, u->manager->load_queue, u);

        if (u->in_dbus_queue)
                LIST_REMOVE(dbus_queue, u->manager->dbus_unit_queue, u);

        if (u->in_cleanup_queue)
                LIST_REMOVE(cleanup_queue, u->manager->cleanup_queue, u);

        if (u->in_gc_queue) {
                LIST_REMOVE(gc_queue, u->manager->gc_queue, u);
                u->manager->n_in_gc_queue--;
        }

        if (u->in_cgroup_queue)
                LIST_REMOVE(cgroup_queue, u->manager->cgroup_queue, u);

        if (u->cgroup_path) {
                hashmap_remove(u->manager->cgroup_unit, u->cgroup_path);
                free(u->cgroup_path);
        }

        free(u->description);
        strv_free(u->documentation);
        free(u->fragment_path);
        free(u->source_path);
        strv_free(u->dropin_paths);
        free(u->instance);

        set_free_free(u->names);

        unit_unwatch_all_pids(u);

        condition_free_list(u->conditions);

        unit_ref_unset(&u->slice);

        while (u->refs)
                unit_ref_unset(u->refs);

        free(u);
}