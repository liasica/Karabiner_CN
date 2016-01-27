// This header intentionally has no include guards.

class url final {
public:
  url(void) : background_(false) {}

  boost::optional<const std::string&> get_name(void) const {
    if (!name_) return boost::none;
    return *name_;
  }
  void set_name(const std::string& v) { name_ = v; }

  boost::optional<const std::string&> get_url(void) const {
    if (!url_) return boost::none;
    return *url_;
  }
  void set_url(const std::string& v) { url_ = v; }

  boost::optional<const std::string&> get_type(void) const {
    if (!type_) return boost::none;
    return *type_;
  }
  void set_type(const std::string& v) { type_ = v; }

  bool get_background(void) const {
    return background_;
  }
  void set_background(bool v) { background_ = v; }

private:
  boost::optional<std::string> name_;
  boost::optional<std::string> url_;
  boost::optional<std::string> type_;
  bool background_;
};

class url_loader final {
public:
  url_loader(const xml_compiler& xml_compiler,
             symbol_map& symbol_map,
             remapclasses_initialize_vector& remapclasses_initialize_vector,
             boost::unordered_map<uint32_t, std::string>& identifier_map,
             boost::unordered_map<uint32_t, std::shared_ptr<url>>& vk_open_url_map) : xml_compiler_(xml_compiler),
                                                                                      symbol_map_(symbol_map),
                                                                                      remapclasses_initialize_vector_(remapclasses_initialize_vector),
                                                                                      identifier_map_(identifier_map),
                                                                                      vk_open_url_map_(vk_open_url_map) {}
  ~url_loader(void);

  void traverse(const extracted_ptree& pt) const;

private:
  const xml_compiler& xml_compiler_;
  symbol_map& symbol_map_;
  remapclasses_initialize_vector& remapclasses_initialize_vector_;
  boost::unordered_map<uint32_t, std::string>& identifier_map_;

  boost::unordered_map<uint32_t, std::shared_ptr<url>>& vk_open_url_map_;
};
