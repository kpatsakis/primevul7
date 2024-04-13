float QuantizedTypeMaxAsFloat(DataType data_type) {
  switch (data_type) {
    case DT_QINT8:
      return Eigen::NumTraits<qint8>::highest();
    case DT_QUINT8:
      return Eigen::NumTraits<quint8>::highest();
    case DT_QINT16:
      return Eigen::NumTraits<qint16>::highest();
    case DT_QUINT16:
      return Eigen::NumTraits<quint16>::highest();
    case DT_QINT32:
      return Eigen::NumTraits<qint32>::highest();
    default:
      return 0.0f;
  }
}