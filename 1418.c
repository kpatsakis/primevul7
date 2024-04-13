DataType GetDataTypeFromNodeOrProps(const NodeDef& node,
                                    const GraphProperties& properties) {
  DataType dtype = DT_INVALID;
  if (node.attr().count("T") == 1) {
    dtype = node.attr().at("T").type();
  } else if (node.attr().count("dtype") == 1) {
    dtype = node.attr().at("dtype").type();
  } else if (IsLogicalOr(node) || IsLogicalAnd(node)) {
    dtype = DT_BOOL;
  } else {
    auto output_props = properties.GetOutputProperties(node.name());
    if (!output_props.empty()) {
      dtype = output_props[0].dtype();
    }
  }
  return dtype;
}