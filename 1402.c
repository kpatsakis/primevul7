float QuantizedTypeMinAsFloat(DataType data_type) {
  switch (data_type) {
    case DT_QINT8:
      return Eigen::NumTraits<qint8>::lowest();
    case DT_QUINT8:
      return Eigen::NumTraits<quint8>::lowest();
    case DT_QINT16:
      return Eigen::NumTraits<qint16>::lowest();
    case DT_QUINT16:
      return Eigen::NumTraits<quint16>::lowest();
    case DT_QINT32:
      return Eigen::NumTraits<qint32>::lowest();
    default:
      return 0.0f;
  }
}