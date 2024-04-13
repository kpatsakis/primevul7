Status ConstantFolding::ReplaceOperationWithConstant(
    double value, const GraphProperties& properties,
    const TensorShapeProto& shape, NodeDef* node, GraphDef* graph) {
  const DataType dtype = GetDataTypeFromNodeOrProps(*node, properties);
  if (dtype == DT_VARIANT) return Status::OK();
  AttrValue tensor_attr;
  Status s = CreateConstantTensorAttrValue(dtype, value, shape, &tensor_attr);
  if (!s.ok()) {
    // Fail gracefully without mutating the graph.
    VLOG(1) << "Failed to replace node " << node->name() << " of type "
            << DataTypeString(dtype) << " with constant tensor of value "
            << value;
    return Status::OK();
  }
  return ReplaceOperationWithConstantTensor(dtype, tensor_attr.mutable_tensor(),
                                            node, graph);
}