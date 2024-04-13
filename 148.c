void QuotaManager::DeleteOriginData(
    const GURL& origin, StorageType type, StatusCallback* callback) {
  LazyInitialize();

  if (origin.is_empty() || clients_.empty()) {
    callback->Run(kQuotaStatusOk);
    delete callback;
    return;
  }

  OriginDataDeleter* deleter =
      new OriginDataDeleter(this, origin, type, callback);
  deleter->Start();
}
