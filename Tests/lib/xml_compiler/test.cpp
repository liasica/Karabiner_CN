#include <boost/property_tree/xml_parser.hpp>
#include <gtest/gtest.h>
#include "pqrs/xml_compiler_bindings_clang.h"
#include "pqrs/xml_compiler.hpp"
#include "bridge.h"

TEST(pqrs_xml_compiler, reload) {
  pqrs::xml_compiler xml_compiler("data/system_xml", "data/private_xml");
  xml_compiler.reload();
  EXPECT_EQ(0, xml_compiler.get_error_information().get_count());
  EXPECT_EQ("", xml_compiler.get_error_information().get_message());

  {
    int v = 0;
    int space_is_ignored = 0;

    EXPECT_EQ(boost::optional<uint32_t>(v++), xml_compiler.get_symbol_map().get_optional("ConfigIndex::system_vk_modifier_definition"));
    EXPECT_EQ(boost::optional<uint32_t>(v++), xml_compiler.get_symbol_map().get_optional("ConfigIndex::system_vk_change_inputsource_definition"));
    EXPECT_EQ(boost::optional<uint32_t>(v++), xml_compiler.get_symbol_map().get_optional("ConfigIndex::system_vk_open_url_definition"));
    EXPECT_EQ(boost::optional<uint32_t>(v++), xml_compiler.get_symbol_map().get_optional("ConfigIndex::notsave_private_sample"));
    EXPECT_EQ(boost::optional<uint32_t>(v++), xml_compiler.get_symbol_map().get_optional("ConfigIndex::notsave_passthrough"));
    EXPECT_EQ(boost::optional<uint32_t>(v++), xml_compiler.get_symbol_map().get_optional("ConfigIndex::notsave_remap_sample"));
    EXPECT_EQ(boost::optional<uint32_t>(v++), xml_compiler.get_symbol_map().get_optional("ConfigIndex::private_include_test"));
    EXPECT_EQ(boost::optional<uint32_t>(v++), xml_compiler.get_symbol_map().get_optional("ConfigIndex::private_style_test_important"));
    EXPECT_EQ(boost::optional<uint32_t>(v++), xml_compiler.get_symbol_map().get_optional("ConfigIndex::private_style_test_caution"));
    EXPECT_EQ(boost::optional<uint32_t>(v++), xml_compiler.get_symbol_map().get_optional("ConfigIndex::private_replacement"));
    space_is_ignored = v;
    EXPECT_EQ(boost::optional<uint32_t>(v++), xml_compiler.get_symbol_map().get_optional("ConfigIndex::private_space_is_ignored"));

    std::string expected = "private.space_ is_ ignored";
    EXPECT_EQ(boost::optional<const std::string&>(expected), xml_compiler.get_identifier(space_is_ignored));
    EXPECT_EQ(boost::optional<int>(space_is_ignored), xml_compiler.get_config_index(expected));
  }

  EXPECT_EQ(boost::optional<uint32_t>(1), xml_compiler.get_symbol_map().get_optional("ModifierFlag::CAPSLOCK"));
  EXPECT_EQ(boost::optional<uint32_t>(2), xml_compiler.get_symbol_map().get_optional("ModifierFlag::SHIFT_L"));
  EXPECT_EQ(boost::optional<uint32_t>(13), xml_compiler.get_symbol_map().get_optional("ModifierFlag::TEST1"));
  EXPECT_NE(boost::none, xml_compiler.get_symbol_map().get_optional("KeyCode::VK_MODIFIER_TEST1"));
  EXPECT_EQ(boost::none, xml_compiler.get_symbol_map().get_optional("KeyCode::VK_MODIFIER_SHIFT_L"));

  EXPECT_EQ(boost::optional<uint32_t>(123), xml_compiler.get_symbol_map().get_optional("KeyCode::MY_INCLUDE_TEST_123"));
  EXPECT_EQ(boost::optional<uint32_t>(456), xml_compiler.get_symbol_map().get_optional("KeyCode::MY_INCLUDE_TEST_456"));
  EXPECT_EQ(boost::optional<uint32_t>(654), xml_compiler.get_symbol_map().get_optional("KeyCode::MY_INCLUDE_TEST_654"));
  EXPECT_EQ(boost::optional<uint32_t>(1), xml_compiler.get_symbol_map().get_optional("KeyCode::MY_INCLUDE_TEST_PARENT1"));
  EXPECT_EQ(boost::optional<uint32_t>(2), xml_compiler.get_symbol_map().get_optional("KeyCode::MY_INCLUDE_TEST_PARENT2"));
  EXPECT_EQ(boost::optional<uint32_t>(3), xml_compiler.get_symbol_map().get_optional("KeyCode::MY_INCLUDE_TEST_PARENT3"));
  EXPECT_EQ(boost::optional<uint32_t>(123), xml_compiler.get_symbol_map().get_optional("KeyCode::MY_LANG_KEY"));
  EXPECT_EQ(boost::optional<uint32_t>(999), xml_compiler.get_symbol_map().get_optional("KeyCode::SPACE_IS_IGNORED"));

  // ------------------------------------------------------------
  uint32_t vk_change_inputsource_base = 1241;

  // JAPANESE
  EXPECT_EQ(boost::optional<uint32_t>(vk_change_inputsource_base),
            xml_compiler.get_symbol_map().get_optional("KeyCode::VK_CHANGE_INPUTSOURCE_JAPANESE"));
  EXPECT_EQ(true,
            xml_compiler.is_vk_change_inputsource_matched(vk_change_inputsource_base,
                                                          "ja",
                                                          "com.apple.inputmethod.Kotoeri.Japanese",
                                                          "com.apple.inputmethod.Japanese"));
  EXPECT_EQ(false,
            xml_compiler.is_vk_change_inputsource_matched(vk_change_inputsource_base,
                                                          "en",
                                                          "com.apple.keylayout.US",
                                                          ""));
  ++vk_change_inputsource_base;

  // DVORAK
  EXPECT_EQ(boost::optional<uint32_t>(vk_change_inputsource_base),
            xml_compiler.get_symbol_map().get_optional("KeyCode::VK_CHANGE_INPUTSOURCE_DVORAK"));
  EXPECT_EQ(true,
            xml_compiler.is_vk_change_inputsource_matched(vk_change_inputsource_base,
                                                          "en",
                                                          "com.apple.keylayout.Dvorak",
                                                          ""));
  EXPECT_EQ(false,
            xml_compiler.is_vk_change_inputsource_matched(vk_change_inputsource_base,
                                                          "en",
                                                          "com.apple.keylayout.US",
                                                          ""));
  ++vk_change_inputsource_base;

  // SWISS
  EXPECT_EQ(boost::optional<uint32_t>(vk_change_inputsource_base),
            xml_compiler.get_symbol_map().get_optional("KeyCode::VK_CHANGE_INPUTSOURCE_SWISS"));
  EXPECT_EQ(true,
            xml_compiler.is_vk_change_inputsource_matched(vk_change_inputsource_base,
                                                          "en",
                                                          "com.apple.keylayout.SwissFrench",
                                                          ""));
  EXPECT_EQ(false,
            xml_compiler.is_vk_change_inputsource_matched(vk_change_inputsource_base,
                                                          "en",
                                                          "com.apple.keylayout.US",
                                                          ""));
  ++vk_change_inputsource_base;

  // Invalid keycode
  EXPECT_EQ(false,
            xml_compiler.is_vk_change_inputsource_matched(vk_change_inputsource_base,
                                                          "",
                                                          "",
                                                          ""));

  // ------------------------------------------------------------
  // inputsourcedef
  {
    std::vector<uint32_t> expect;
    expect.push_back(*(xml_compiler.get_symbol_map().get_optional("InputSource::CANADIAN")));
    expect.push_back(*(xml_compiler.get_symbol_map().get_optional("InputSource::ENGLISH")));

    std::vector<uint32_t> actual;
    for (size_t i = 0; i < xml_compiler.get_inputsource_vector_size(); ++i) {
      uint32_t inputsource = 0;
      if (xml_compiler.is_inputsource_matched(inputsource,
                                              i,
                                              "en",
                                              "com.apple.keylayout.Canadian",
                                              "")) {
        actual.push_back(inputsource);
      }
    }

    EXPECT_EQ(expect, actual);
  }

#if 0
  {
    uint32_t inputsource;
    xml_compiler.get_inputsourceid(inputsource,
                                   "en",
                                   "com.apple.keylayout.Canadian",
                                   "");
    EXPECT_EQ(xml_compiler.get_symbol_map().get_optional("InputSource::CANADIAN"), inputsource);
    EXPECT_TRUE(xml_compiler.get_symbol_map().get_optional("InputSource::FRENCH") != inputsource);
  }

  {
    uint32_t inputsource;
    xml_compiler.get_inputsourceid(inputsource,
                                   "",
                                   "com.apple.keyboardlayout.fr-dvorak-bepo.keylayout.FrenchDvorak",
                                   "");
    EXPECT_EQ(xml_compiler.get_symbol_map().get_optional("InputSource::BEPO"), inputsource);
  }

  {
    uint32_t inputsource;
    xml_compiler.get_inputsourceid(inputsource,
                                   "",
                                   "com.apple.keyboardlayout.fr-dvorak-bepo.keylayout.FrenchDvorak-AzertyCmd",
                                   "");
    EXPECT_EQ(xml_compiler.get_symbol_map().get_optional("InputSource::BEPO"), inputsource);
  }

  {
    uint32_t inputsource;
    xml_compiler.get_inputsourceid(inputsource,
                                   "ja",
                                   "com.apple.inputmethod.Kotoeri.Japanese",
                                   "com.apple.inputmethod.Japanese");
    EXPECT_EQ(xml_compiler.get_symbol_map().get_optional("InputSource::JAPANESE"), inputsource);
  }

  {
    uint32_t inputsource;
    xml_compiler.get_inputsourceid(inputsource,
                                   "fr",
                                   "com.apple.keylayout.French",
                                   "");
    EXPECT_EQ(xml_compiler.get_symbol_map().get_optional("InputSource::FRENCH"), inputsource);
  }

  {
    uint32_t inputsource;
    xml_compiler.get_inputsourceid(inputsource,
                                   "en",
                                   "com.apple.keylayout.US",
                                   "");
    EXPECT_EQ(xml_compiler.get_symbol_map().get_optional("InputSource::MY_ENGLISH"), inputsource);
  }
#endif

  // ------------------------------------------------------------
  // vkopenurldef
  uint32_t vk_open_url_base = vk_change_inputsource_base;

  {
    EXPECT_EQ(boost::optional<uint32_t>(vk_open_url_base),
              xml_compiler.get_symbol_map().get_optional("KeyCode::VK_OPEN_URL_WEB_pqrs_org"));
    ++vk_open_url_base;
  }

  {
    EXPECT_EQ(boost::optional<uint32_t>(vk_open_url_base),
              xml_compiler.get_symbol_map().get_optional("KeyCode::VK_OPEN_URL_APP_TextEdit"));
    ++vk_open_url_base;
  }

  {
    EXPECT_EQ(boost::optional<uint32_t>(vk_open_url_base),
              xml_compiler.get_symbol_map().get_optional("KeyCode::VK_OPEN_URL_SHELL_date_pbcopy"));
    ++vk_open_url_base;
  }

  // ------------------------------------------------------------
  uint32_t vk_config_base = vk_open_url_base;
  EXPECT_EQ(boost::optional<uint32_t>(vk_config_base++),
            xml_compiler.get_symbol_map().get_optional("KeyCode::VK_CONFIG_TOGGLE_notsave_passthrough"));
  EXPECT_EQ(boost::optional<uint32_t>(vk_config_base++),
            xml_compiler.get_symbol_map().get_optional("KeyCode::VK_CONFIG_FORCE_ON_notsave_passthrough"));
  EXPECT_EQ(boost::optional<uint32_t>(vk_config_base++),
            xml_compiler.get_symbol_map().get_optional("KeyCode::VK_CONFIG_FORCE_OFF_notsave_passthrough"));
  EXPECT_EQ(boost::optional<uint32_t>(vk_config_base++),
            xml_compiler.get_symbol_map().get_optional("KeyCode::VK_CONFIG_SYNC_KEYDOWNUP_notsave_passthrough"));

  EXPECT_EQ(boost::optional<uint32_t>(2), xml_compiler.get_symbol_map().get_optional("ConsumerKeyCode::BRIGHTNESS_UP"));
  EXPECT_EQ("ConsumerKeyCode::BRIGHTNESS_UP", *(xml_compiler.get_symbol_map().get_name("ConsumerKeyCode", 2)));
  EXPECT_EQ(boost::none, xml_compiler.get_symbol_map().get_name("ConsumerKeyCode", 12345));

  // system 6 + private 4 - duplicated 2 == 8
  EXPECT_EQ(8, xml_compiler.get_app_vector_size());
  // system 70 + private 1 == 71
  EXPECT_EQ(71, xml_compiler.get_inputsource_vector_size());
  // system 2 + private 0 == 2
  EXPECT_EQ(2, xml_compiler.get_window_name_vector_size());

  // ---------------------------------------
  {
    std::vector<uint32_t> expect;
    expect.push_back(*(xml_compiler.get_symbol_map().get_optional("ApplicationType::TERMINAL_APPLE")));
    expect.push_back(*(xml_compiler.get_symbol_map().get_optional("ApplicationType::TERMINAL")));

    std::vector<uint32_t> actual;
    for (size_t i = 0; i < xml_compiler.get_app_vector_size(); ++i) {
      uint32_t appid = 0;
      if (xml_compiler.is_app_matched(appid, i, "com.apple.Terminal")) {
        actual.push_back(appid);
      }
    }

    EXPECT_EQ(expect, actual);
  }

  {
    std::vector<uint32_t> expect;
    expect.push_back(*(xml_compiler.get_symbol_map().get_optional("ApplicationType::VI")));

    std::vector<uint32_t> actual;
    for (size_t i = 0; i < xml_compiler.get_app_vector_size(); ++i) {
      uint32_t appid = 0;
      if (xml_compiler.is_app_matched(appid, i, "org.vim.MacVim")) {
        actual.push_back(appid);
      }
    }

    EXPECT_EQ(expect, actual);
  }

  {
    std::vector<uint32_t> expect;

    std::vector<uint32_t> actual;
    for (size_t i = 0; i < xml_compiler.get_app_vector_size(); ++i) {
      uint32_t appid = 0;
      if (xml_compiler.is_app_matched(appid, i, "com.googlecode.iterm2")) {
        actual.push_back(appid);
      }
    }

    EXPECT_EQ(expect, actual);
  }

  {
    std::vector<uint32_t> expect;
    expect.push_back(*(xml_compiler.get_symbol_map().get_optional("ApplicationType::EMACS")));

    std::vector<uint32_t> actual;
    for (size_t i = 0; i < xml_compiler.get_app_vector_size(); ++i) {
      uint32_t appid = 0;
      if (xml_compiler.is_app_matched(appid, i, "org.gnu.Emacs")) {
        actual.push_back(appid);
      }
    }

    EXPECT_EQ(expect, actual);
  }

  {
    std::vector<uint32_t> expect;

    std::vector<uint32_t> actual;
    for (size_t i = 0; i < xml_compiler.get_app_vector_size(); ++i) {
      uint32_t appid = 0;
      if (xml_compiler.is_app_matched(appid, i, "org.gnu.AquamacsEmacs")) {
        actual.push_back(appid);
      }
    }

    EXPECT_EQ(expect, actual);
  }

  {
    std::vector<uint32_t> expect;
    expect.push_back(*(xml_compiler.get_symbol_map().get_optional("ApplicationType::PREFIX_TEST")));

    std::vector<uint32_t> actual;
    for (size_t i = 0; i < xml_compiler.get_app_vector_size(); ++i) {
      uint32_t appid = 0;
      if (xml_compiler.is_app_matched(appid, i, "org.pqrs.prefix.test")) {
        actual.push_back(appid);
      }
    }

    EXPECT_EQ(expect, actual);
  }

  {
    std::vector<uint32_t> expect;
    expect.push_back(*(xml_compiler.get_symbol_map().get_optional("ApplicationType::PREFIX_TEST")));

    std::vector<uint32_t> actual;
    for (size_t i = 0; i < xml_compiler.get_app_vector_size(); ++i) {
      uint32_t appid = 0;
      if (xml_compiler.is_app_matched(appid, i, "org.pqrs.prefix.test1111")) {
        actual.push_back(appid);
      }
    }

    EXPECT_EQ(expect, actual);
  }

  {
    std::vector<uint32_t> expect;

    std::vector<uint32_t> actual;
    for (size_t i = 0; i < xml_compiler.get_app_vector_size(); ++i) {
      uint32_t appid = 0;
      if (xml_compiler.is_app_matched(appid, i, "dummy.org.pqrs.prefix.test")) {
        actual.push_back(appid);
      }
    }

    EXPECT_EQ(expect, actual);
  }

  {
    std::vector<uint32_t> expect;
    expect.push_back(*(xml_compiler.get_symbol_map().get_optional("ApplicationType::SUFFIX_TEST")));

    std::vector<uint32_t> actual;
    for (size_t i = 0; i < xml_compiler.get_app_vector_size(); ++i) {
      uint32_t appid = 0;
      if (xml_compiler.is_app_matched(appid, i, "test.suffix.pqrs.org")) {
        actual.push_back(appid);
      }
    }

    EXPECT_EQ(expect, actual);
  }

  {
    std::vector<uint32_t> expect;
    expect.push_back(*(xml_compiler.get_symbol_map().get_optional("ApplicationType::SUFFIX_TEST")));

    std::vector<uint32_t> actual;
    for (size_t i = 0; i < xml_compiler.get_app_vector_size(); ++i) {
      uint32_t appid = 0;
      if (xml_compiler.is_app_matched(appid, i, "1111test.suffix.pqrs.org")) {
        actual.push_back(appid);
      }
    }

    EXPECT_EQ(expect, actual);
  }

  {
    std::vector<uint32_t> expect;

    std::vector<uint32_t> actual;
    for (size_t i = 0; i < xml_compiler.get_app_vector_size(); ++i) {
      uint32_t appid = 0;
      if (xml_compiler.is_app_matched(appid, i, "test.suffix.pqrs.org.dummy")) {
        actual.push_back(appid);
      }
    }

    EXPECT_EQ(expect, actual);
  }

  // ---------------------------------------
  {
    std::vector<uint32_t> expect;
    expect.push_back(*(xml_compiler.get_symbol_map().get_optional("InputSource::MY_ENGLISH")));
    expect.push_back(*(xml_compiler.get_symbol_map().get_optional("InputSource::ENGLISH")));

    std::vector<uint32_t> actual;
    for (size_t i = 0; i < xml_compiler.get_inputsource_vector_size(); ++i) {
      uint32_t inputsource = 0;
      if (xml_compiler.is_inputsource_matched(inputsource,
                                              i,
                                              "en",
                                              "com.apple.keylayout.US",
                                              "")) {
        actual.push_back(inputsource);
      }
    }

    EXPECT_EQ(expect, actual);
  }

  {
    std::vector<uint32_t> expect;
    expect.push_back(*(xml_compiler.get_symbol_map().get_optional("WindowName::Gmail")));
    expect.push_back(*(xml_compiler.get_symbol_map().get_optional("WindowName::Google")));

    std::vector<uint32_t> actual;
    for (size_t i = 0; i < xml_compiler.get_window_name_vector_size(); ++i) {
      uint32_t windownameid;

      if (xml_compiler.is_window_name_matched(windownameid,
                                              i,
                                              "Example - Gmail")) {
        actual.push_back(windownameid);
      }
    }

    EXPECT_EQ(expect, actual);
  }

  EXPECT_EQ(boost::optional<uint32_t>(6), xml_compiler.get_symbol_map().get_optional("ApplicationType::SPACE_IS_IGNORED"));

  EXPECT_EQ(boost::optional<uint32_t>(0x03f0), xml_compiler.get_symbol_map().get_optional("DeviceVendor::HEWLETT_PACKARD"));
  EXPECT_EQ(boost::optional<uint32_t>(0x9999), xml_compiler.get_symbol_map().get_optional("DeviceVendor::SPACE_IS_IGNORED"));
  EXPECT_EQ(boost::optional<uint32_t>(0x0224), xml_compiler.get_symbol_map().get_optional("DeviceProduct::MY_HP_KEYBOARD"));
  EXPECT_EQ(boost::optional<uint32_t>(0x9999), xml_compiler.get_symbol_map().get_optional("DeviceProduct::SPACE_IS_IGNORED"));

  EXPECT_EQ(boost::none, xml_compiler.get_url(0));

  EXPECT_EQ("https://pqrs.org/",
            *(xml_compiler.get_url(*(xml_compiler.get_symbol_map().get_optional("KeyCode::VK_OPEN_URL_WEB_pqrs_org")))));
  EXPECT_EQ(boost::none,
            xml_compiler.get_url_type(*(xml_compiler.get_symbol_map().get_optional("KeyCode::VK_OPEN_URL_WEB_pqrs_org"))));

  EXPECT_EQ("file:///Applications/TextEdit.app",
            *(xml_compiler.get_url(*(xml_compiler.get_symbol_map().get_optional("KeyCode::VK_OPEN_URL_APP_TextEdit")))));
  EXPECT_EQ(boost::none,
            xml_compiler.get_url_type(*(xml_compiler.get_symbol_map().get_optional("KeyCode::VK_OPEN_URL_WEB_pqrs_org"))));

  EXPECT_EQ("/bin/date | /usr/bin/pbcopy",
            *(xml_compiler.get_url(*(xml_compiler.get_symbol_map().get_optional("KeyCode::VK_OPEN_URL_SHELL_date_pbcopy")))));
  EXPECT_EQ("shell",
            *(xml_compiler.get_url_type(*(xml_compiler.get_symbol_map().get_optional("KeyCode::VK_OPEN_URL_SHELL_date_pbcopy")))));

  auto node_tree = xml_compiler.get_preferences_checkbox_node_tree();
  EXPECT_TRUE(node_tree.get_children() != nullptr);
  {
    auto node_ptr = (*(node_tree.get_children()))[0];
    EXPECT_EQ("Swap Space and Tab\n  appendix1\n  appendix123\n  appendix123_2", node_ptr->get_node().get_name());
  }
  {
    auto node_ptr = (*(node_tree.get_children()))[1];
    EXPECT_EQ("style test: important", node_ptr->get_node().get_name());
    EXPECT_EQ("important", node_ptr->get_node().get_style());
  }
  {
    auto node_ptr = (*(node_tree.get_children()))[2];
    EXPECT_EQ("style test: caution", node_ptr->get_node().get_name());
    EXPECT_EQ("caution", node_ptr->get_node().get_style());
  }

  {
    std::vector<uint32_t> actual;
    EXPECT_TRUE(xml_compiler.debug_get_initialize_vector(actual, "remap.showstatusmessage"));

    std::vector<uint32_t> expected;

    // <autogen>__ShowStatusMessage__ A B C D E F G -- </autogen>

    uint32_t buffer[5];
    memset(&buffer, 0, sizeof(buffer));
    strlcpy(reinterpret_cast<char*>(buffer), "A B C D E F G --", sizeof(buffer));

    expected.push_back(6); // count
    expected.push_back(BRIDGE_STATUSMESSAGE);
    expected.push_back(buffer[0]); // "A B "
    expected.push_back(buffer[1]); // "C D "
    expected.push_back(buffer[2]); // "E F "
    expected.push_back(buffer[3]); // "G --"
    expected.push_back(buffer[4]); // \0

    EXPECT_EQ(expected, actual);
  }

  {
    std::vector<uint32_t> actual;
    EXPECT_TRUE(xml_compiler.debug_get_initialize_vector(actual, "remap.showstatusmessage_old_style"));

    std::vector<uint32_t> expected;

    // <autogen>--ShowStatusMessage-- O L D 1 2 3 4 - -- </autogen>

    uint32_t buffer[6];
    memset(&buffer, 0, sizeof(buffer));
    strlcpy(reinterpret_cast<char*>(buffer), "O L D 1 2 3 4 - --", sizeof(buffer));

    expected.push_back(6); // count
    expected.push_back(BRIDGE_STATUSMESSAGE);
    expected.push_back(buffer[0]); // "O L "
    expected.push_back(buffer[1]); // "D 1 "
    expected.push_back(buffer[2]); // "2 3 "
    expected.push_back(buffer[3]); // "4 - "
    expected.push_back(buffer[4]); // "--" \0\0

    EXPECT_EQ(expected, actual);
  }

  {
    std::vector<uint32_t> actual;
    EXPECT_TRUE(xml_compiler.debug_get_initialize_vector(actual, "remap.use_separator"));

    std::vector<uint32_t> expected;

    // <autogen>
    //   __KeyOverlaidModifier__
    //   KeyCode::SPACE,
    //
    //   KeyCode::RETURN, KeyCode::TAB,
    //
    //   Option::SEPARATOR,
    //
    //   KeyCode::SPACE,
    // </autogen>

    expected.push_back(13); // count
    expected.push_back(BRIDGE_REMAPTYPE_KEYOVERLAIDMODIFIER);

    expected.push_back(BRIDGE_DATATYPE_OPTION);
    expected.push_back(1); // Option::USE_SEPARATOR

    expected.push_back(BRIDGE_DATATYPE_KEYCODE);
    expected.push_back(49); // space

    expected.push_back(BRIDGE_DATATYPE_KEYCODE);
    expected.push_back(36); // return

    expected.push_back(BRIDGE_DATATYPE_KEYCODE);
    expected.push_back(48); // tab

    expected.push_back(BRIDGE_DATATYPE_OPTION);
    expected.push_back(2); // Option::SEPARATOR

    expected.push_back(BRIDGE_DATATYPE_KEYCODE);
    expected.push_back(49); // space

    EXPECT_EQ(expected, actual);
  }

  {
    std::vector<uint32_t> actual;
    EXPECT_TRUE(xml_compiler.debug_get_initialize_vector(actual, "remap.without_separator"));

    std::vector<uint32_t> expected;

    // <autogen>
    //   __KeyOverlaidModifier__
    //   KeyCode::SPACE,
    //
    //   KeyCode::RETURN,
    //
    //   KeyCode::TAB, KeyCode::SPACE,
    // </autogen>

    expected.push_back(9); // count
    expected.push_back(BRIDGE_REMAPTYPE_KEYOVERLAIDMODIFIER);

    expected.push_back(BRIDGE_DATATYPE_KEYCODE);
    expected.push_back(49); // space

    expected.push_back(BRIDGE_DATATYPE_KEYCODE);
    expected.push_back(36); // return

    expected.push_back(BRIDGE_DATATYPE_KEYCODE);
    expected.push_back(48); // tab

    expected.push_back(BRIDGE_DATATYPE_KEYCODE);
    expected.push_back(49); // space

    EXPECT_EQ(expected, actual);
  }
}

TEST(pqrs_xml_compiler, reload_bindings_clang) {
  pqrs_xml_compiler* p = nullptr;
  EXPECT_EQ(0, pqrs_xml_compiler_initialize(&p, "data/system_xml", "data/private_xml"));
  pqrs_xml_compiler_reload(p, "checkbox.xml");

  {
    std::vector<uint32_t> expect;
    expect.push_back(pqrs_xml_compiler_get_symbol_map_value(p, "InputSource::CANADIAN"));
    expect.push_back(pqrs_xml_compiler_get_symbol_map_value(p, "InputSource::ENGLISH"));

    std::vector<uint32_t> actual;
    for (size_t i = 0; i < pqrs_xml_compiler_get_inputsource_vector_size(p); ++i) {
      uint32_t inputsource = 0;
      if (pqrs_xml_compiler_is_inputsource_matched(p,
                                                   &inputsource,
                                                   i,
                                                   "en",
                                                   "com.apple.keylayout.Canadian",
                                                   "")) {
        actual.push_back(inputsource);
      }
    }

    EXPECT_EQ(expect, actual);
  }

  {
    std::vector<uint32_t> expect;

    std::vector<uint32_t> actual;
    for (size_t i = 0; i < pqrs_xml_compiler_get_inputsource_vector_size(p); ++i) {
      uint32_t inputsource = 0;
      if (pqrs_xml_compiler_is_inputsource_matched(p,
                                                   &inputsource,
                                                   i,
                                                   nullptr,
                                                   nullptr,
                                                   nullptr)) {
        actual.push_back(inputsource);
      }
    }

    EXPECT_EQ(expect, actual);
  }

  pqrs_xml_compiler_terminate(&p);
}

TEST(pqrs_xml_compiler, reload_invalid_xml) {
  // ------------------------------------------------------------
  // invalid XML format
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/broken_xml");
    xml_compiler.reload();
    EXPECT_EQ("<data/invalid_xml/broken_xml/private.xml>(4): expected element name",
              xml_compiler.get_error_information().get_message());
    EXPECT_EQ(boost::optional<uint32_t>(2), xml_compiler.get_symbol_map().get_optional("ConsumerKeyCode::BRIGHTNESS_UP"));
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/broken_include");
    xml_compiler.reload();
    EXPECT_EQ("<data/invalid_xml/broken_include/include.xml>(4): expected element name",
              xml_compiler.get_error_information().get_message());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/missing_include");
    xml_compiler.reload();

    EXPECT_EQ(0, xml_compiler.get_error_information().get_count());

    auto node_tree = xml_compiler.get_preferences_checkbox_node_tree();
    EXPECT_TRUE(node_tree.get_children() != nullptr);

    auto node_ptr = (*(node_tree.get_children()))[0];
    EXPECT_EQ("Caution:\n  data/invalid_xml/missing_include/include.xml is not found.", node_ptr->get_node().get_name());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/infinite_include_loop");
    xml_compiler.reload();
    EXPECT_EQ("An infinite include loop is detected:\n"
              "data/invalid_xml/infinite_include_loop/private.xml",
              xml_compiler.get_error_information().get_message());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/infinite_include_loop2");
    xml_compiler.reload();
    EXPECT_EQ("An infinite include loop is detected:\n"
              "data/invalid_xml/infinite_include_loop2/include.xml",
              xml_compiler.get_error_information().get_message());
  }

  // ------------------------------------------------------------
  // identifier
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/dup_identifier");
    xml_compiler.reload();
    const char* message = "Duplicated identifier:\n"
                          "\n"
                          "<identifier>private.swap_space_and_tab</identifier>";
    EXPECT_EQ(message, xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/empty_identifier");
    xml_compiler.reload();
    EXPECT_EQ("Empty <identifier>.", xml_compiler.get_error_information().get_message());
    EXPECT_EQ(2, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/empty_identifier2");
    xml_compiler.reload();
    EXPECT_EQ("Empty <identifier>.", xml_compiler.get_error_information().get_message());
    EXPECT_EQ(2, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/invalid_identifier_place");
    xml_compiler.reload();
    const char* message = "<identifier> must be placed directly under <item>:\n"
                          "\n"
                          "<identifier>private.swap_space_and_tab</identifier>";
    EXPECT_EQ(message, xml_compiler.get_error_information().get_message());
    EXPECT_EQ(2, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/invalid_identifier_place2");
    xml_compiler.reload();
    const char* message = "<identifier> must be placed directly under <item>:\n"
                          "\n"
                          "<identifier>private.swap_space_and_tab</identifier>";
    EXPECT_EQ(message, xml_compiler.get_error_information().get_message());
    EXPECT_EQ(2, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/invalid_identifier_place3");
    xml_compiler.reload();
    const char* message = "You should not write <identifier> in <item> which has child <item> nodes.\n"
                          "Remove <identifier>private.invalid_identifier_place3</identifier>.";
    EXPECT_EQ(message, xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }

  // ------------------------------------------------------------
  // autogen
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/unknown_autogen");
    xml_compiler.reload();
    const char* message = "Invalid <autogen>:\n"
                          "\n"
                          "<autogen>__KeyToKey2__ KeyCode::SPACE, VK_SHIFT, KeyCode::TAB</autogen>";
    EXPECT_EQ(message, xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());

    {
      std::vector<uint32_t> actual;
      EXPECT_TRUE(xml_compiler.debug_get_initialize_vector(actual, "private.swap_space_and_tab"));

      // empty if error
      std::vector<uint32_t> expected;
      EXPECT_EQ(expected, actual);
    }
    {
      std::vector<uint32_t> actual;
      EXPECT_TRUE(xml_compiler.debug_get_initialize_vector(actual, "private.swap_space_and_tab2"));

      std::vector<uint32_t> expected;

      // ------------------------------------------------------------
      // <autogen>__KeyToKey__ KeyCode::SPACE, VK_SHIFT, KeyCode::TAB</autogen>
      expected.push_back(9); // count
      expected.push_back(BRIDGE_REMAPTYPE_KEYTOKEY);
      expected.push_back(BRIDGE_DATATYPE_KEYCODE);
      expected.push_back(49); // KeyCode::SPACE
      expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAG);
      expected.push_back(2); // ModifierFlag::SHIFT_L
      expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAGS_END);
      expected.push_back(1);
      expected.push_back(BRIDGE_DATATYPE_KEYCODE);
      expected.push_back(48); // KeyCode::TAB

      expected.push_back(9); // count
      expected.push_back(BRIDGE_REMAPTYPE_KEYTOKEY);
      expected.push_back(BRIDGE_DATATYPE_KEYCODE);
      expected.push_back(49); // KeyCode::SPACE
      expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAG);
      expected.push_back(3); // ModifierFlag::SHIFT_R
      expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAGS_END);
      expected.push_back(1);
      expected.push_back(BRIDGE_DATATYPE_KEYCODE);
      expected.push_back(48); // KeyCode::TAB

      // ------------------------------------------------------------
      // <autogen>--KeyToKey-- KeyCode::TAB, VK_SHIFT, KeyCode::SPACE</autogen>
      expected.push_back(9); // count
      expected.push_back(BRIDGE_REMAPTYPE_KEYTOKEY);
      expected.push_back(BRIDGE_DATATYPE_KEYCODE);
      expected.push_back(48); // KeyCode::TAB
      expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAG);
      expected.push_back(2); // ModifierFlag::SHIFT_L
      expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAGS_END);
      expected.push_back(1);
      expected.push_back(BRIDGE_DATATYPE_KEYCODE);
      expected.push_back(49); // KeyCode::SPACE

      expected.push_back(9); // count
      expected.push_back(BRIDGE_REMAPTYPE_KEYTOKEY);
      expected.push_back(BRIDGE_DATATYPE_KEYCODE);
      expected.push_back(48); // KeyCode::TAB
      expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAG);
      expected.push_back(3); // ModifierFlag::SHIFT_R
      expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAGS_END);
      expected.push_back(1);
      expected.push_back(BRIDGE_DATATYPE_KEYCODE);
      expected.push_back(49); // KeyCode::SPACE

      EXPECT_EQ(expected, actual);
    }
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/autogen_invalid_pipe_type");
    xml_compiler.reload();
    const char* message = "Cannot connect(|) except ModifierFlag:\n"
                          "\n"
                          "KeyCode::SPACE|KeyCode::TAB";
    EXPECT_EQ(message, xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }

  // ------------------------------------------------------------
  // Unknown symbol_map
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/unknown_symbol_map");
    xml_compiler.reload();
    EXPECT_EQ("Unknown symbol:\n\nKeyCode::MY_UNKNOWN_KEY", xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }

  // ------------------------------------------------------------
  // Unknown data_type
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/unknown_data_type");
    xml_compiler.reload();
    EXPECT_EQ("Unknown symbol:\n\nKeyCode2::SPACE", xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }

  // ------------------------------------------------------------
  // replacementdef.xml
  {
    pqrs::xml_compiler xml_compiler("data/invalid_xml/replacementdef_no_name", "data/private_xml");
    xml_compiler.reload();
    EXPECT_EQ("No <replacementname> within <replacementdef>.", xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/invalid_xml/replacementdef_empty_name", "data/private_xml");
    xml_compiler.reload();
    EXPECT_EQ("Empty <replacementname> within <replacementdef>.", xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/invalid_xml/replacementdef_invalid_name1", "data/private_xml");
    xml_compiler.reload();
    EXPECT_EQ("<data/invalid_xml/replacementdef_invalid_name1/replacementdef.xml>(60): unexpected end of data",
              xml_compiler.get_error_information().get_message());
    EXPECT_EQ(2, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/invalid_xml/replacementdef_invalid_name2", "data/private_xml");
    xml_compiler.reload();
    EXPECT_EQ("Do not use '{{' and '}}' within <replacementname>:\n\nVI_}}J", xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/invalid_xml/replacementdef_no_value", "data/private_xml");
    xml_compiler.reload();
    EXPECT_EQ("No <replacementvalue> within <replacementdef>:\n\nVI_J", xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/invalid_xml/replacementdef_not_found", "data/private_xml");
    xml_compiler.reload();
    EXPECT_EQ("Warning - \"APPDEF_REPLACEMENT\" is not found in replacement.\n", xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }

  // ------------------------------------------------------------
  // symbol_map.xml
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/symbol_map_xml_no_type");
    xml_compiler.reload();
    EXPECT_EQ("No 'type' Attribute within <symbol_map>.", xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/symbol_map_xml_empty_type");
    xml_compiler.reload();
    EXPECT_EQ("Empty 'type' Attribute within <symbol_map>.", xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/symbol_map_xml_no_name");
    xml_compiler.reload();
    EXPECT_EQ("No 'name' Attribute within <symbol_map>.", xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/symbol_map_xml_empty_name");
    xml_compiler.reload();
    EXPECT_EQ("Empty 'name' Attribute within <symbol_map>.", xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/symbol_map_xml_no_value");
    xml_compiler.reload();
    EXPECT_EQ("No 'value' Attribute within <symbol_map>.", xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/symbol_map_xml_empty_value");
    xml_compiler.reload();
    EXPECT_EQ("Empty 'value' Attribute within <symbol_map>.", xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/symbol_map_xml_invalid_value");
    xml_compiler.reload();
    const char* message = "Invalid 'value' Attribute within <symbol_map>:\n"
                          "\n"
                          "<symbol_map type=\"ConsumerKeyCode\" name=\"BRIGHTNESS_UP\" value=\"XXX\" />";
    EXPECT_EQ(message, xml_compiler.get_error_information().get_message());
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }

  // ------------------------------------------------------------
  // modifierdef.xml
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/modifierdef_empty");
    xml_compiler.reload();
    EXPECT_EQ("Empty <modifierdef>.", std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/modifierdef_invalid_notify");
    xml_compiler.reload();
    EXPECT_EQ("Invalid 'notify' attribute within <modifierdef>: falsee",
              std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }

  // ------------------------------------------------------------
  // appdef.xml
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/appdef_no_name");
    xml_compiler.reload();
    EXPECT_EQ("No <appname> within <appdef>.", std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/appdef_empty_name");
    xml_compiler.reload();
    EXPECT_EQ("Empty <appname> within <appdef>.", std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }

  // ------------------------------------------------------------
  // windownamedef.xml
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/windownamedef_no_name");
    xml_compiler.reload();
    EXPECT_EQ("No <name> within <windownamedef>.", std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/windownamedef_empty_name");
    xml_compiler.reload();
    EXPECT_EQ("Empty <name> within <windownamedef>.", std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }

  // ------------------------------------------------------------
  // uielementroledef.xml
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/uielementroledef_empty_name");
    xml_compiler.reload();
    EXPECT_EQ("Empty <uielementroledef>.", std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }

  // ------------------------------------------------------------
  // devicevendordef.xml
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/devicevendordef_no_name");
    xml_compiler.reload();
    EXPECT_EQ("No <vendorname> within <devicevendordef>.", std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/devicevendordef_empty_name");
    xml_compiler.reload();
    EXPECT_EQ("Empty <vendorname> within <devicevendordef>.", std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/devicevendordef_no_value");
    xml_compiler.reload();
    EXPECT_EQ("No <vendorid> within <devicevendordef>.", std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/devicevendordef_empty_value");
    xml_compiler.reload();
    EXPECT_EQ("Empty <vendorid> within <devicevendordef>.", std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/devicevendordef_invalid_value");
    xml_compiler.reload();
    const char* message = "Invalid <vendorid> within <devicevendordef>:\n"
                          "\n"
                          "<vendorid>XXX</vendorid>";
    EXPECT_EQ(message, std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }

  // ------------------------------------------------------------
  // deviceproductdef.xml
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/deviceproductdef_no_name");
    xml_compiler.reload();
    EXPECT_EQ("No <productname> within <deviceproductdef>.", std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/deviceproductdef_empty_name");
    xml_compiler.reload();
    EXPECT_EQ("Empty <productname> within <deviceproductdef>.", std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/deviceproductdef_no_value");
    xml_compiler.reload();
    EXPECT_EQ("No <productid> within <deviceproductdef>.", std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/deviceproductdef_empty_value");
    xml_compiler.reload();
    EXPECT_EQ("Empty <productid> within <deviceproductdef>.", std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/deviceproductdef_invalid_value");
    xml_compiler.reload();
    const char* message = "Invalid <productid> within <deviceproductdef>:\n"
                          "\n"
                          "<productid>XXX</productid>";
    EXPECT_EQ(message, std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }

  // ------------------------------------------------------------
  // inputsourcedef.xml
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/vkchangeinputsourcedef_no_name");
    xml_compiler.reload();
    EXPECT_EQ("No <name> within <vkchangeinputsourcedef>.", std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/vkchangeinputsourcedef_empty_name");
    xml_compiler.reload();
    EXPECT_EQ("<name> within <vkchangeinputsourcedef> must start with \"KeyCode::VK_CHANGE_INPUTSOURCE_\":\n"
              "\n"
              "<name></name>",
              std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }

  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/inputsourcedef_no_name");
    xml_compiler.reload();
    EXPECT_EQ("No <name> within <inputsourcedef>.", std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
  {
    pqrs::xml_compiler xml_compiler("data/system_xml", "data/invalid_xml/inputsourcedef_empty_name");
    xml_compiler.reload();
    EXPECT_EQ("Empty <name> within <inputsourcedef>.",
              std::string(xml_compiler.get_error_information().get_message()));
    EXPECT_EQ(1, xml_compiler.get_error_information().get_count());
  }
}

TEST(pqrs_xml_compiler_symbol_map, add) {
  pqrs::xml_compiler::symbol_map s;
  s.add("KeyCode", "SPACE", 36);
  s.add("KeyCode", "VK__AUTOINDEX__BEGIN__", 1024);
  s.add("KeyCode", "VK_NEW1");
  s.add("KeyCode", "VK_NEW2");

  EXPECT_EQ(static_cast<uint32_t>(1024), s.get("KeyCode::VK_NEW1"));
  EXPECT_EQ(static_cast<uint32_t>(1025), s.get("KeyCode::VK_NEW2"));
}

TEST(pqrs_xml_compiler_remapclasses_initialize_vector, get) {
  pqrs::xml_compiler::remapclasses_initialize_vector v;
  EXPECT_EQ(1, v.get().size());
}

TEST(pqrs_xml_compiler_remapclasses_initialize_vector, add) {
  pqrs::xml_compiler::remapclasses_initialize_vector v;
  v.clear();
  v.start(1, "remap.empty");
  v.end();

  v.start(3, "remap.three_items");
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  v.end();

  v.freeze();

  std::vector<uint32_t> expected;
  expected.push_back(4); // count is {0,1,2,3}

  expected.push_back(1); // configindex:1
  expected.push_back(1);

  expected.push_back(4); // configindex:3
  expected.push_back(3);
  expected.push_back(1);
  expected.push_back(2);
  expected.push_back(3);

  expected.push_back(1); // configindex:0
  expected.push_back(0);

  expected.push_back(1); // configindex:2
  expected.push_back(2);

  EXPECT_EQ(expected, v.get());
}

TEST(pqrs_xml_compiler_remapclasses_initialize_vector, add_partial) {
  pqrs::xml_compiler::remapclasses_initialize_vector v;
  v.clear();
  v.start(1, "remap.empty");
  v.end();

  v.start(3, "remap.three_items");
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  // v.end();

  v.start(4, "remap.two_items");
  v.push_back(1);
  v.push_back(2);
  v.end();

  v.freeze();

  std::vector<uint32_t> expected;
  expected.push_back(5); // count is {0,1,2,3,4}

  expected.push_back(1); // configindex:1
  expected.push_back(1);

  expected.push_back(3); // configindex:4
  expected.push_back(4);
  expected.push_back(1);
  expected.push_back(2);

  expected.push_back(1); // configindex:0
  expected.push_back(0);

  expected.push_back(1); // configindex:2
  expected.push_back(2);

  expected.push_back(1); // configindex:3
  expected.push_back(3);

  EXPECT_EQ(expected, v.get());
}

TEST(pqrs_xml_compiler_filter_vector, filter_vector) {
  pqrs::xml_compiler::symbol_map s;
  s.add("ApplicationType", "APP1", 1);
  s.add("ApplicationType", "APP2", 2);
  s.add("ApplicationType", "APP3", 3);
  s.add("WindowName", "WN1", 5);
  s.add("WindowName", "WN2", 6);
  s.add("WindowName", "WN3", 7);
  s.add("UIElementRole", "UER1", 20001);
  s.add("UIElementRole", "UER2", 20002);
  s.add("UIElementRole", "UER3", 20003);
  s.add("DeviceVendor", "VENDOR1", 10);
  s.add("DeviceVendor", "VENDOR2", 20);
  s.add("DeviceVendor", "VENDOR3", 30);
  s.add("DeviceProduct", "ANY", 0);
  s.add("DeviceProduct", "PRODUCT1", 100);
  s.add("DeviceProduct", "PRODUCT2", 200);
  s.add("DeviceProduct", "PRODUCT3", 300);
  s.add("DeviceLocation", "ANY", 0);
  s.add("DeviceLocation", "LOCATION1", 400);
  s.add("DeviceLocation", "LOCATION2", 500);
  s.add("DeviceLocation", "LOCATION3", 600);
  s.add("InputSource", "NONE", 0);
  s.add("InputSource", "INPUTSOURCE1", 110);
  s.add("InputSource", "INPUTSOURCE2", 111);
  s.add("InputSource", "INPUTSOURCE3", 112);
  s.add("ConfigIndex", "config1", 1000);
  s.add("ConfigIndex", "config2", 2000);
  s.add("ConfigIndex", "config3", 3000);
  s.add("ModifierFlag", "MOD1", 0x1000);
  s.add("ModifierFlag", "MOD2", 0x2000);
  s.add("ModifierFlag", "MOD3", 0x4000);
  s.add("KeyCode", "KC1", 4000);
  s.add("KeyCode", "KC2", 5000);
  s.add("KeyCode", "KC3", 6000);
  s.add("EventType", "DOWN", 10);
  s.add("EventType", "UP", 11);

  std::string xml("<?xml version=\"1.0\"?>"
                  "<item>"
                  "  <only>APP1,APP3</only>"
                  "  <only><!-- empty --></only>"
                  "  <only>,,</only>"
                  "  <not><!-- XXX --->APP2</not>"
                  "  <windowname_only>WN1,WN2</windowname_only>"
                  "  <windowname_not>WN3</windowname_not>"
                  "  <uielementrole_only>UER1,UER2</uielementrole_only>"
                  "  <uielementrole_not>UER3</uielementrole_not>"
                  "  <identifier>sample</identifier>"
                  "  <device_only>DeviceVendor::VENDOR1, DeviceProduct::PRODUCT1, DeviceLocation::LOCATION1</device_only>"
                  "  <device_not>"
                  "    DeviceVendor::VENDOR3,,,,"
                  "    DeviceProduct::PRODUCT3,"
                  "    DeviceVendor::VENDOR3,"
                  "  </device_not>"
                  "  <device_not></device_not>"
                  "  <inputsource_only>INPUTSOURCE1</inputsource_only>"
                  "  <inputsource_not>INPUTSOURCE2, INPUTSOURCE3</inputsource_not>"
                  "  <inputsourcedetail_only>INPUTSOURCE1</inputsourcedetail_only>"
                  "  <inputsourcedetail_not>INPUTSOURCE2, INPUTSOURCE3</inputsourcedetail_not>"
                  "  <inputmode_only>INPUTSOURCE1</inputmode_only>"
                  "  <inputmode_not>INPUTSOURCE2, INPUTSOURCE3</inputmode_not>"
                  "  <inputmodedetail_only>INPUTSOURCE1</inputmodedetail_only>"
                  "  <inputmodedetail_not>INPUTSOURCE2, INPUTSOURCE3</inputmodedetail_not>"
                  "  <config_only>config1,config2</config_only>"
                  "  <config_not>config3</config_not>"
                  "  <modifier_only>ModifierFlag::MOD1 ||| ModifierFlag::MOD3</modifier_only>"
                  "  <modifier_not> ModifierFlag::MOD2 </modifier_not>"
                  "  <modifierlocked_only>ModifierFlag::MOD1 ||| ModifierFlag::MOD3</modifierlocked_only>"
                  "  <modifierlocked_not> ModifierFlag::MOD2 </modifierlocked_not>"
                  "  <modifierstuck_only>ModifierFlag::MOD1 ||| ModifierFlag::MOD3</modifierstuck_only>"
                  "  <modifierstuck_not> ModifierFlag::MOD2 </modifierstuck_not>"
                  "  <lastpressedphysicalkey_not>KeyCode::KC1</lastpressedphysicalkey_not>"
                  "  <lastpressedphysicalkey_only>KeyCode::KC2</lastpressedphysicalkey_only>"
                  "  <lastreleasedphysicalkey_not>KeyCode::KC1</lastreleasedphysicalkey_not>"
                  "  <lastreleasedphysicalkey_only>KeyCode::KC2</lastreleasedphysicalkey_only>"
                  "  <lastsentevent_not>KeyCode::KC1, EventType::DOWN</lastsentevent_not>"
                  "  <lastsentevent_only>KeyCode::KC2, EventType::UP</lastsentevent_only>"
                  "  <elapsedtimesincelastpressed_greaterthan>"
                  "    Millisecond::RawValue::1000"
                  "  </elapsedtimesincelastpressed_greaterthan>"
                  "  <elapsedtimesincelastpressed_lessthan>"
                  "    Millisecond::RawValue::500"
                  "  </elapsedtimesincelastpressed_lessthan>"
                  "  <elapsedtimesincelastreleased_greaterthan>"
                  "    Millisecond::RawValue::1000"
                  "  </elapsedtimesincelastreleased_greaterthan>"
                  "  <elapsedtimesincelastreleased_lessthan>"
                  "    Millisecond::RawValue::500"
                  "  </elapsedtimesincelastreleased_lessthan>"
                  "  <pressingphysicalkeys_greaterthan>1</pressingphysicalkeys_greaterthan>"
                  "  <pressingphysicalkeys_lessthan>2</pressingphysicalkeys_lessthan>"
                  "</item>");
  std::stringstream istream(xml, std::stringstream::in);

  pqrs::xml_compiler xml_compiler("data/system_xml", "data/private_xml");

  int flags = boost::property_tree::xml_parser::no_comments;
  boost::property_tree::ptree pt;
  boost::property_tree::read_xml(istream, pt, flags);

  pqrs::string::replacement replacement;

  for (auto& it : pt) {
    pqrs::xml_compiler::filter_vector fv(s);
    fv.traverse(pqrs::xml_compiler::extracted_ptree(xml_compiler, replacement, it.second, ""));

    std::vector<uint32_t> expected;

    // <only>APP1,APP3</only>
    expected.push_back(3); // count
    expected.push_back(BRIDGE_FILTERTYPE_APPLICATION_ONLY);
    expected.push_back(1); // APP1
    expected.push_back(3); // APP3

    // <only><!-- empty --></only>"
    // <only>,,</only>"
    // ***IGNORED***

    // <not>APP2</not>
    expected.push_back(2); // count
    expected.push_back(BRIDGE_FILTERTYPE_APPLICATION_NOT);
    expected.push_back(2); // APP2

    // <windowname_only>WN1,WN2</windowname_only>
    expected.push_back(3); // count
    expected.push_back(BRIDGE_FILTERTYPE_WINDOWNAME_ONLY);
    expected.push_back(5); // WN1
    expected.push_back(6); // WN2

    // <windowname_not>WN3</windowname_not>
    expected.push_back(2); // count
    expected.push_back(BRIDGE_FILTERTYPE_WINDOWNAME_NOT);
    expected.push_back(7); // WN3

    // <uielementrole_only>UER1,UER2</uielementrole_only>
    expected.push_back(3); // count
    expected.push_back(BRIDGE_FILTERTYPE_UIELEMENTROLE_ONLY);
    expected.push_back(20001); // UER1
    expected.push_back(20002); // UER2

    // <uielementrole_not>UER3</uielementrole_not>
    expected.push_back(2); // count
    expected.push_back(BRIDGE_FILTERTYPE_UIELEMENTROLE_NOT);
    expected.push_back(20003); // UER3

    // <device_only>DeviceVendor::VENDOR1, DeviceProduct::PRODUCT1, DeviceLocation::LOCATION1</device_only>
    expected.push_back(4); // count
    expected.push_back(BRIDGE_FILTERTYPE_DEVICE_ONLY);
    expected.push_back(10);
    expected.push_back(100);
    expected.push_back(400);

    // <device_not>DeviceVendor::VENDOR3, DeviceProduct::PRODUCT3, DeviceVendor::VENDOR3</device_not>
    expected.push_back(7); // count
    expected.push_back(BRIDGE_FILTERTYPE_DEVICE_NOT);
    expected.push_back(30);
    expected.push_back(300);
    expected.push_back(0); // DeviceLocation::ANY
    expected.push_back(30);
    expected.push_back(0); // DeviceProduct::ANY
    expected.push_back(0); // DeviceLocation::ANY

    // <device_not></device_not>
    // ***IGNORED***

    // <inputsource_only>InputSource::INPUTSOURCE1</inputsource_only>
    expected.push_back(2); // count
    expected.push_back(BRIDGE_FILTERTYPE_INPUTSOURCE_ONLY);
    expected.push_back(110);

    // <inputsource_not>InputSource::INPUTSOURCE2, InputSource::INPUTSOURCE3</inputsource_not>
    expected.push_back(3); // count
    expected.push_back(BRIDGE_FILTERTYPE_INPUTSOURCE_NOT);
    expected.push_back(111);
    expected.push_back(112);

    // <inputsourcedetail_only>InputSource::INPUTSOURCE1</inputsourcedetail_only>
    expected.push_back(2); // count
    expected.push_back(BRIDGE_FILTERTYPE_INPUTSOURCE_ONLY);
    expected.push_back(110);

    // <inputsourcedetail_not>InputSource::INPUTSOURCE2, InputSource::INPUTSOURCE3</inputsourcedetail_not>
    expected.push_back(3); // count
    expected.push_back(BRIDGE_FILTERTYPE_INPUTSOURCE_NOT);
    expected.push_back(111);
    expected.push_back(112);

    // <inputmode_only>INPUTSOURCE1</inputmode_only>
    expected.push_back(2); // count
    expected.push_back(BRIDGE_FILTERTYPE_INPUTSOURCE_ONLY);
    expected.push_back(110);

    // <inputmode_not>INPUTSOURCE2, INPUTSOURCE3</inputmode_not>
    expected.push_back(3); // count
    expected.push_back(BRIDGE_FILTERTYPE_INPUTSOURCE_NOT);
    expected.push_back(111);
    expected.push_back(112);

    // <inputmodedetail_only>INPUTSOURCE1</inputmodedetail_only>
    expected.push_back(2); // count
    expected.push_back(BRIDGE_FILTERTYPE_INPUTSOURCE_ONLY);
    expected.push_back(110);

    // <inputmodedetail_not>INPUTSOURCE2, INPUTSOURCE3</inputmodedetail_not>
    expected.push_back(3); // count
    expected.push_back(BRIDGE_FILTERTYPE_INPUTSOURCE_NOT);
    expected.push_back(111);
    expected.push_back(112);

    // <config_only>config1,config2</config_only>
    expected.push_back(3); // count
    expected.push_back(BRIDGE_FILTERTYPE_CONFIG_ONLY);
    expected.push_back(1000);
    expected.push_back(2000);

    // <config_not>config3</config_not>
    expected.push_back(2); // count
    expected.push_back(BRIDGE_FILTERTYPE_CONFIG_NOT);
    expected.push_back(3000);

    // <modifier_only>ModifierFlag::MOD1 ||| ModifierFlag::MOD3</modifier_only>
    expected.push_back(7);
    expected.push_back(BRIDGE_FILTERTYPE_MODIFIER_ONLY);
    expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAG);
    expected.push_back(0x1000);
    expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAG);
    expected.push_back(0x4000);
    expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAGS_END);
    expected.push_back(1);

    // <modifier_not> ModifierFlag::MOD2 </modifier_not>
    expected.push_back(5);
    expected.push_back(BRIDGE_FILTERTYPE_MODIFIER_NOT);
    expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAG);
    expected.push_back(0x2000);
    expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAGS_END);
    expected.push_back(1);

    // <modifierlocked_only>ModifierFlag::MOD1 ||| ModifierFlag::MOD3</modifierlocked_only>
    expected.push_back(7);
    expected.push_back(BRIDGE_FILTERTYPE_MODIFIER_LOCKED_ONLY);
    expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAG);
    expected.push_back(0x1000);
    expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAG);
    expected.push_back(0x4000);
    expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAGS_END);
    expected.push_back(1);

    // <modifierlocked_not> ModifierFlag::MOD2 </modifierlocked_not>
    expected.push_back(5);
    expected.push_back(BRIDGE_FILTERTYPE_MODIFIER_LOCKED_NOT);
    expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAG);
    expected.push_back(0x2000);
    expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAGS_END);
    expected.push_back(1);

    // <modifierstuck_only>ModifierFlag::MOD1 ||| ModifierFlag::MOD3</modifierstuck_only>
    expected.push_back(7);
    expected.push_back(BRIDGE_FILTERTYPE_MODIFIER_STUCK_ONLY);
    expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAG);
    expected.push_back(0x1000);
    expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAG);
    expected.push_back(0x4000);
    expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAGS_END);
    expected.push_back(1);

    // <modifierstuck_not> ModifierFlag::MOD2 </modifierstuck_not>
    expected.push_back(5);
    expected.push_back(BRIDGE_FILTERTYPE_MODIFIER_STUCK_NOT);
    expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAG);
    expected.push_back(0x2000);
    expected.push_back(BRIDGE_DATATYPE_MODIFIERFLAGS_END);
    expected.push_back(1);

    // <lastpressedphysicalkey_not>KeyCode::KC1</lastpressedphysicalkey_not>
    expected.push_back(3);
    expected.push_back(BRIDGE_FILTERTYPE_LASTPRESSEDPHYSICALKEY_NOT);
    expected.push_back(BRIDGE_DATATYPE_KEYCODE);
    expected.push_back(4000);

    // <lastpressedphysicalkey_only>KeyCode::KC2</lastpressedphysicalkey_only>
    expected.push_back(3);
    expected.push_back(BRIDGE_FILTERTYPE_LASTPRESSEDPHYSICALKEY_ONLY);
    expected.push_back(BRIDGE_DATATYPE_KEYCODE);
    expected.push_back(5000);

    // <lastreleasedphysicalkey_not>KeyCode::KC1</lastreleasedphysicalkey_not>
    expected.push_back(3);
    expected.push_back(BRIDGE_FILTERTYPE_LASTRELEASEDPHYSICALKEY_NOT);
    expected.push_back(BRIDGE_DATATYPE_KEYCODE);
    expected.push_back(4000);

    // <lastreleasedphysicalkey_only>KeyCode::KC2</lastreleasedphysicalkey_only>
    expected.push_back(3);
    expected.push_back(BRIDGE_FILTERTYPE_LASTRELEASEDPHYSICALKEY_ONLY);
    expected.push_back(BRIDGE_DATATYPE_KEYCODE);
    expected.push_back(5000);

    // <lastsentevent_not>KeyCode::KC1, EventType::DOWN</lastsentevent_not>
    expected.push_back(5);
    expected.push_back(BRIDGE_FILTERTYPE_LASTSENTEVENT_NOT);
    expected.push_back(BRIDGE_DATATYPE_KEYCODE);
    expected.push_back(4000);
    expected.push_back(BRIDGE_DATATYPE_EVENTTYPE);
    expected.push_back(10);

    // <lastsentevent_only>KeyCode::KC2, EventType::UP</lastsentevent_only>
    expected.push_back(5);
    expected.push_back(BRIDGE_FILTERTYPE_LASTSENTEVENT_ONLY);
    expected.push_back(BRIDGE_DATATYPE_KEYCODE);
    expected.push_back(5000);
    expected.push_back(BRIDGE_DATATYPE_EVENTTYPE);
    expected.push_back(11);

    // <elapsedtimesincelastpressed_greaterthan>
    //   Millisecond::RawValue::1000
    // </elapsedtimesincelastpressed_greaterthan>
    expected.push_back(3);
    expected.push_back(BRIDGE_FILTERTYPE_ELAPSEDTIMESINCELASTPRESSED_GREATERTHAN);
    expected.push_back(BRIDGE_DATATYPE_MILLISECOND);
    expected.push_back(1000);

    // <elapsedtimesincelastpressed_lessthan>
    //   Millisecond::RawValue::500
    // </elapsedtimesincelastpressed_lessthan>
    expected.push_back(3);
    expected.push_back(BRIDGE_FILTERTYPE_ELAPSEDTIMESINCELASTPRESSED_LESSTHAN);
    expected.push_back(BRIDGE_DATATYPE_MILLISECOND);
    expected.push_back(500);

    // <elapsedtimesincelastreleased_greaterthan>
    //   Millisecond::RawValue::1000
    // </elapsedtimesincelastreleased_greaterthan>
    expected.push_back(3);
    expected.push_back(BRIDGE_FILTERTYPE_ELAPSEDTIMESINCELASTRELEASED_GREATERTHAN);
    expected.push_back(BRIDGE_DATATYPE_MILLISECOND);
    expected.push_back(1000);

    // <elapsedtimesincelastreleased_lessthan>
    //   Millisecond::RawValue::500
    // </elapsedtimesincelastreleased_lessthan>
    expected.push_back(3);
    expected.push_back(BRIDGE_FILTERTYPE_ELAPSEDTIMESINCELASTRELEASED_LESSTHAN);
    expected.push_back(BRIDGE_DATATYPE_MILLISECOND);
    expected.push_back(500);

    //  <pressingphysicalkeys_greaterthan>1</pressingphysicalkeys_greaterthan>"
    expected.push_back(2);
    expected.push_back(BRIDGE_FILTERTYPE_PRESSINGPHYSICALKEYS_GREATERTHAN);
    expected.push_back(1);

    //  <pressingphysicalkeys_lessthan>2</pressingphysicalkeys_lessthan>"
    expected.push_back(2);
    expected.push_back(BRIDGE_FILTERTYPE_PRESSINGPHYSICALKEYS_LESSTHAN);
    expected.push_back(2);

    EXPECT_EQ(expected, fv.get());
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
