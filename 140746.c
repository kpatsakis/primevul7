    if (iTask.task == NULL) {
        qemu_mutex_unlock(&iscsilun->mutex);
        return -ENOMEM;
    }