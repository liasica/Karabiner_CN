#include <exception>
#include "pqrs/xml_compiler.hpp"

namespace pqrs {
void xml_compiler::device_loader::traverse(const extracted_ptree& pt) const {
  for (const auto& it : pt) {
    if (it.get_tag_name() != "devicevendordef" &&
        it.get_tag_name() != "deviceproductdef" &&
        it.get_tag_name() != "devicelocationdef") {
      if (!it.children_empty()) {
        traverse(it.children_extracted_ptree());
      }
    } else {
      std::string type;
      const char* name_tag_name = nullptr;
      const char* value_tag_name = nullptr;
      boost::optional<std::string> name;
      boost::optional<std::string> value;

      // ----------------------------------------
      if (it.get_tag_name() == "devicevendordef") {
        type = "DeviceVendor";
        name_tag_name = "vendorname";
        value_tag_name = "vendorid";

      } else if (it.get_tag_name() == "deviceproductdef") {
        type = "DeviceProduct";
        name_tag_name = "productname";
        value_tag_name = "productid";

      } else if (it.get_tag_name() == "devicelocationdef") {
        type = "DeviceLocation";
        name_tag_name = "locationname";
        value_tag_name = "locationid";

      } else {
        assert(!"unknown type in device_loader::traverse");
      }

      // ----------------------------------------
      for (const auto& child : it.children_extracted_ptree()) {
        if (child.get_tag_name() == name_tag_name) {
          name = pqrs::string::remove_whitespaces_copy(child.get_data());
        } else if (child.get_tag_name() == value_tag_name) {
          value = boost::trim_copy(child.get_data());
        }
      }

      // ----------------------------------------
      // Validation
      if (!name) {
        xml_compiler_.error_information_.set(boost::format("No <%1%> within <%2%>.") %
                                             name_tag_name %
                                             it.get_tag_name());
        continue;
      }

      if (name->empty()) {
        xml_compiler_.error_information_.set(boost::format("Empty <%1%> within <%2%>.") %
                                             name_tag_name %
                                             it.get_tag_name());
        continue;
      }

      if (!value) {
        xml_compiler_.error_information_.set(boost::format("No <%1%> within <%2%>.") %
                                             value_tag_name %
                                             it.get_tag_name());
        continue;
      }

      if (value->empty()) {
        xml_compiler_.error_information_.set(boost::format("Empty <%1%> within <%2%>.") %
                                             value_tag_name %
                                             it.get_tag_name());
        continue;
      }

      auto v = pqrs::string::to_uint32_t(value);
      if (!v) {
        xml_compiler_.error_information_.set(boost::format("Invalid <%1%> within <%2%>:\n\n<%3%>%4%</%5%>") %
                                             value_tag_name %
                                             it.get_tag_name() %
                                             value_tag_name %
                                             *value %
                                             value_tag_name);
        continue;
      }

      // ----------------------------------------
      symbol_map_.add(type, *name, *v);
    }
  }
}
}
