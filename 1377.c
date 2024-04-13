bool AllValuesAre(const TensorProto& proto, const T& value) {
  Tensor tensor;
  if (!tensor.FromProto(proto)) {
    return false;
  }
  auto values = tensor.flat<T>();
  for (int i = 0; i < tensor.NumElements(); ++i) {
    if (values(i) != value) {
      return false;
    }
  }
  return true;
}