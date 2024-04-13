void unit_add_to_gc_queue(Unit *u) {
        assert(u);

        if (u->in_gc_queue || u->in_cleanup_queue)
                return;

        if (unit_check_gc(u))
                return;

        LIST_PREPEND(gc_queue, u->manager->gc_queue, u);
        u->in_gc_queue = true;

        u->manager->n_in_gc_queue ++;
}