char *unit_default_cgroup_path(Unit *u) {
        _cleanup_free_ char *escaped = NULL, *slice = NULL;
        int r;

        assert(u);

        if (unit_has_name(u, SPECIAL_ROOT_SLICE))
                return strdup(u->manager->cgroup_root);

        if (UNIT_ISSET(u->slice) && !unit_has_name(UNIT_DEREF(u->slice), SPECIAL_ROOT_SLICE)) {
                r = cg_slice_to_path(UNIT_DEREF(u->slice)->id, &slice);
                if (r < 0)
                        return NULL;
        }

        escaped = cg_escape(u->id);
        if (!escaped)
                return NULL;

        if (slice)
                return strjoin(u->manager->cgroup_root, "/", slice, "/", escaped, NULL);
        else
                return strjoin(u->manager->cgroup_root, "/", escaped, NULL);
}