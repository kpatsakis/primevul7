static bool unit_condition_test(Unit *u) {
        assert(u);

        dual_timestamp_get(&u->condition_timestamp);
        u->condition_result = condition_test_list(u->id, u->conditions);

        return u->condition_result;
}