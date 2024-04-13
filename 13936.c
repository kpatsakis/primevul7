SPICE_DESTRUCTOR_FUNC(reds_exit)
{
    GList *l;

    pthread_mutex_lock(&global_reds_lock);
    for (l = servers; l != NULL; l = l->next) {
        RedsState *reds = (RedsState*) l->data;
        reds_cleanup(reds);
    }
    pthread_mutex_unlock(&global_reds_lock);
}