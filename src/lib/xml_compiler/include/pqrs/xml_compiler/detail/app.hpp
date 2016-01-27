// This header intentionally has no include guards.

class app final {
public:
  boost::optional<const std::string&> get_name(void) const {
    if (!name_) return boost::none;
    return *name_;
  }
  void set_name(const std::string& v) { name_ = v; }
  void add_rule_equal(const std::string& v);
  void add_rule_prefix(const std::string& v);
  void add_rule_suffix(const std::string& v);
  bool is_rules_matched(const std::string& identifier) const;

private:
  boost::optional<std::string> name_;
  std::vector<std::string> rules_equal_;
  std::vector<std::string> rules_prefix_;
  std::vector<std::string> rules_suffix_;
};

class app_loader final {
public:
  app_loader(const xml_compiler& xml_compiler,
             symbol_map& symbol_map,
             std::vector<std::shared_ptr<app>>& app_vector) : xml_compiler_(xml_compiler),
                                                              symbol_map_(symbol_map),
                                                              app_vector_(app_vector) {}

  void traverse(const extracted_ptree& pt) const;

private:
  bool exists(const std::string& name) const;

  const xml_compiler& xml_compiler_;
  symbol_map& symbol_map_;
  std::vector<std::shared_ptr<app>>& app_vector_;
};
