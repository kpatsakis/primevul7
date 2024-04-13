void QuotaManager::DeleteOnCorrectThread() const {
  if (!io_thread_->BelongsToCurrentThread()) {
    io_thread_->DeleteSoon(FROM_HERE, this);
    return;
  }
  delete this;
}
