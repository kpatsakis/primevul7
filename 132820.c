int ipmi_smi_watcher_unregister(struct ipmi_smi_watcher *watcher)
{
	mutex_lock(&smi_watchers_mutex);
	list_del(&watcher->link);
	mutex_unlock(&smi_watchers_mutex);
	return 0;
}