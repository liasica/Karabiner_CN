#ifndef MODIFIERNAME_HPP
#define MODIFIERNAME_HPP

#include "IOLogWrapper.hpp"
#include "KeyCode.hpp"
#include "strlcpy_utf8.hpp"

namespace org_pqrs_Karabiner {
class ModifierName final {
public:
  class Item final {
  public:
    Item(void) {
      name_[0] = '\0';
    }

    Item(ModifierFlag modifierFlag, const char* name) : modifierFlag_(modifierFlag) {
      if (name) {
        pqrs::strlcpy_utf8::strlcpy(name_, name, sizeof(name_));
      }
    }

    ModifierFlag getModifierFlag(void) const { return modifierFlag_; }
    const char* getName(void) const { return name_; }

  private:
    enum {
      MAXLEN = 32,
    };

    ModifierFlag modifierFlag_;
    char name_[MAXLEN];
  };
  DECLARE_VECTOR(Item);

  static void initialize(void) {
    clearVirtualModifiers();
  }

  static void clearVirtualModifiers(void);

  static void registerVirtualModifier(ModifierFlag modifierFlag, const char* name) {
    items_.push_back(Item(modifierFlag, name));
  }

  static const char* getName(ModifierFlag modifierFlag);

private:
  static Vector_Item items_;
};
}

#endif
