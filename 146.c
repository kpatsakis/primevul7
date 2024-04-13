QuotaManager::UsageAndQuotaDispatcherTask::Create(
    QuotaManager* manager, const std::string& host, StorageType type) {
  switch (type) {
    case kStorageTypeTemporary:
      return new UsageAndQuotaDispatcherTaskForTemporary(
          manager, host);
    case kStorageTypePersistent:
      return new UsageAndQuotaDispatcherTaskForPersistent(
          manager, host);
    default:
      NOTREACHED();
  }
  return NULL;
}
