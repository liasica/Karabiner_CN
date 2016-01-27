#include <ostream>
#include <gtest/gtest.h>
#include "Config.hpp"
#include "FlagStatus.hpp"
#include "KeyCode.hpp"
#include "KeyCodeModifierFlagPairs.hpp"

using namespace org_pqrs_Karabiner;
Config config;

std::ostream& operator<<(std::ostream& os, const EventType& v) { return os << v.get(); }
std::ostream& operator<<(std::ostream& os, const KeyboardType& v) { return os << v.get(); }
std::ostream& operator<<(std::ostream& os, const ModifierFlag& v) { return os << v.getRawBits(); }
std::ostream& operator<<(std::ostream& os, const Flags& v) { return os << v.get(); }
std::ostream& operator<<(std::ostream& os, const KeyCode& v) { return os << v.get(); }
std::ostream& operator<<(std::ostream& os, const ConsumerKeyCode& v) { return os << v.get(); }
std::ostream& operator<<(std::ostream& os, const PointingButton& v) { return os << v.get(); }
std::ostream& operator<<(std::ostream& os, const Buttons& v) { return os << v.get(); }

Flags operator|(ModifierFlag lhs, ModifierFlag rhs) { return Flags(lhs.getRawBits() | rhs.getRawBits()); }

TEST(Generic, setUp) {
  KeyCodeModifierFlagPairs::clearVirtualModifiers();
}

TEST(FlagStatus, makeFlags) {
  FlagStatus flagStatus;
  EXPECT_EQ(Flags(), flagStatus.makeFlags());

  flagStatus.set();
  EXPECT_EQ(Flags(), flagStatus.makeFlags());

  flagStatus.set(KeyCode::A, Flags(0));
  EXPECT_EQ(Flags(), flagStatus.makeFlags());

  // down SHIFT_L
  flagStatus.set(KeyCode::SHIFT_L, Flags(ModifierFlag::SHIFT_L));
  EXPECT_EQ(Flags(ModifierFlag::SHIFT_L), flagStatus.makeFlags());

  // no effect with ModifierFlag::NONE
  flagStatus.set(KeyCode::A, Flags(ModifierFlag::NONE));
  EXPECT_EQ(Flags(ModifierFlag::SHIFT_L), flagStatus.makeFlags());

  // down CONTROL_
  flagStatus.set(KeyCode::CONTROL_L, ModifierFlag::CONTROL_L | ModifierFlag::SHIFT_L);
  EXPECT_EQ(Flags(ModifierFlag::SHIFT_L | ModifierFlag::CONTROL_L), flagStatus.makeFlags());

  // down A
  flagStatus.set(KeyCode::A, ModifierFlag::CONTROL_L | ModifierFlag::SHIFT_L);
  EXPECT_EQ(Flags(ModifierFlag::SHIFT_L | ModifierFlag::CONTROL_L), flagStatus.makeFlags());

  // up SHIFT_L
  flagStatus.set(KeyCode::SHIFT_L, Flags(ModifierFlag::CONTROL_L));
  EXPECT_EQ(Flags(ModifierFlag::CONTROL_L), flagStatus.makeFlags());

  // up CONTROL_L
  flagStatus.set(KeyCode::CONTROL_L, Flags(0));
  EXPECT_EQ(Flags(), flagStatus.makeFlags());

  // All flags
  flagStatus.reset();
  flagStatus.set(KeyCode::CAPSLOCK, Flags(ModifierFlag::CAPSLOCK));
  EXPECT_EQ(Flags(ModifierFlag::CAPSLOCK), flagStatus.makeFlags());

  flagStatus.set(KeyCode::CAPSLOCK, Flags(0));
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());

  flagStatus.reset();
  flagStatus.set(KeyCode::SHIFT_L, Flags(ModifierFlag::SHIFT_L));
  EXPECT_EQ(Flags(ModifierFlag::SHIFT_L), flagStatus.makeFlags());

  flagStatus.reset();
  flagStatus.set(KeyCode::SHIFT_R, Flags(ModifierFlag::SHIFT_R));
  EXPECT_EQ(Flags(ModifierFlag::SHIFT_R), flagStatus.makeFlags());

  flagStatus.reset();
  flagStatus.set(KeyCode::CONTROL_L, Flags(ModifierFlag::CONTROL_L));
  EXPECT_EQ(Flags(ModifierFlag::CONTROL_L), flagStatus.makeFlags());

  flagStatus.reset();
  flagStatus.set(KeyCode::CONTROL_R, Flags(ModifierFlag::CONTROL_R));
  EXPECT_EQ(Flags(ModifierFlag::CONTROL_R), flagStatus.makeFlags());

  flagStatus.reset();
  flagStatus.set(KeyCode::OPTION_L, Flags(ModifierFlag::OPTION_L));
  EXPECT_EQ(Flags(ModifierFlag::OPTION_L), flagStatus.makeFlags());

  flagStatus.reset();
  flagStatus.set(KeyCode::OPTION_R, Flags(ModifierFlag::OPTION_R));
  EXPECT_EQ(Flags(ModifierFlag::OPTION_R), flagStatus.makeFlags());

  flagStatus.reset();
  flagStatus.set(KeyCode::COMMAND_L, Flags(ModifierFlag::COMMAND_L));
  EXPECT_EQ(Flags(ModifierFlag::COMMAND_L), flagStatus.makeFlags());

  flagStatus.reset();
  flagStatus.set(KeyCode::COMMAND_R, Flags(ModifierFlag::COMMAND_R));
  EXPECT_EQ(Flags(ModifierFlag::COMMAND_R), flagStatus.makeFlags());

  flagStatus.reset();
  flagStatus.set(KeyCode::FN, Flags(ModifierFlag::FN));
  EXPECT_EQ(Flags(ModifierFlag::FN), flagStatus.makeFlags());
}

TEST(FlagStatus, getFlag) {
  FlagStatus flagStatus;

  EXPECT_EQ(ModifierFlag::CAPSLOCK, flagStatus.getFlag(0));
}

TEST(FlagStatus, increase) {
  {
    FlagStatus flagStatus;

    // Do nothing with ModifierFlag::NONE.
    flagStatus.increase(ModifierFlag::NONE);
    EXPECT_EQ(Flags(0), flagStatus.makeFlags());

    flagStatus.increase(ModifierFlag::SHIFT_L);
    EXPECT_EQ(Flags(ModifierFlag::SHIFT_L), flagStatus.makeFlags());

    {
      Vector_ModifierFlag v;
      v.push_back(ModifierFlag::COMMAND_L);
      v.push_back(ModifierFlag::CONTROL_L);
      flagStatus.increase(v);
      EXPECT_EQ(Flags(ModifierFlag::COMMAND_L | ModifierFlag::CONTROL_L | ModifierFlag::SHIFT_L), flagStatus.makeFlags());
    }

    flagStatus.increase(ModifierFlag::NONE);
    EXPECT_EQ(Flags(ModifierFlag::COMMAND_L | ModifierFlag::CONTROL_L | ModifierFlag::SHIFT_L), flagStatus.makeFlags());
  }

  {
    FlagStatus flagStatus;
    Vector_ModifierFlag v;
    v.push_back(ModifierFlag::COMMAND_L);
    v.push_back(ModifierFlag::CONTROL_L);
    flagStatus.increase(ModifierFlag::SHIFT_L, v);
    EXPECT_EQ(Flags(ModifierFlag::COMMAND_L | ModifierFlag::CONTROL_L | ModifierFlag::SHIFT_L), flagStatus.makeFlags());
  }
  {
    FlagStatus flagStatus;
    Vector_ModifierFlag v;
    v.push_back(ModifierFlag::COMMAND_L);
    v.push_back(ModifierFlag::CONTROL_L);
    flagStatus.increase(ModifierFlag::COMMAND_L, v);
    EXPECT_EQ(Flags(ModifierFlag::COMMAND_L | ModifierFlag::CONTROL_L), flagStatus.makeFlags());
    flagStatus.decrease(v);
    EXPECT_EQ(Flags(0), flagStatus.makeFlags());
  }
}

TEST(FlagStatus, decrease) {
  FlagStatus flagStatus;

  {
    Vector_ModifierFlag v;
    v.push_back(ModifierFlag::COMMAND_L);
    v.push_back(ModifierFlag::CONTROL_L);
    flagStatus.increase(v);
    EXPECT_EQ(Flags(ModifierFlag::COMMAND_L | ModifierFlag::CONTROL_L), flagStatus.makeFlags());
  }

  flagStatus.decrease(ModifierFlag::CONTROL_L);
  EXPECT_EQ(Flags(ModifierFlag::COMMAND_L), flagStatus.makeFlags());
}

TEST(FlagStatus, temporary_increase) {
  FlagStatus flagStatus;

  // Do nothing with ModifierFlag::NONE.
  flagStatus.temporary_increase(ModifierFlag::NONE);
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());

  {
    Vector_ModifierFlag v;
    v.push_back(ModifierFlag::COMMAND_L);
    v.push_back(ModifierFlag::CONTROL_L);
    flagStatus.increase(v);
    EXPECT_EQ(Flags(ModifierFlag::COMMAND_L | ModifierFlag::CONTROL_L), flagStatus.makeFlags());
  }

  flagStatus.temporary_increase(ModifierFlag::OPTION_L);
  EXPECT_EQ(Flags(ModifierFlag::COMMAND_L | ModifierFlag::CONTROL_L | ModifierFlag::OPTION_L), flagStatus.makeFlags());

  // temporary_increase will reset by flagStatus.set
  flagStatus.set(KeyCode::COMMAND_L, Flags(ModifierFlag::COMMAND_L));
  EXPECT_EQ(Flags(ModifierFlag::COMMAND_L | ModifierFlag::CONTROL_L), flagStatus.makeFlags());
}

TEST(FlagStatus, temporary_decrease) {
  FlagStatus flagStatus;

  {
    Vector_ModifierFlag v;
    v.push_back(ModifierFlag::COMMAND_L);
    v.push_back(ModifierFlag::CONTROL_L);
    flagStatus.increase(v);
    EXPECT_EQ(Flags(ModifierFlag::COMMAND_L | ModifierFlag::CONTROL_L), flagStatus.makeFlags());
  }

  flagStatus.temporary_decrease(ModifierFlag::CONTROL_L);
  EXPECT_EQ(Flags(ModifierFlag::COMMAND_L), flagStatus.makeFlags());

  // temporary_increase will reset by flagStatus.set
  flagStatus.set(KeyCode::COMMAND_L, Flags(ModifierFlag::COMMAND_L));
  EXPECT_EQ(Flags(ModifierFlag::COMMAND_L | ModifierFlag::CONTROL_L), flagStatus.makeFlags());
}

TEST(FlagStatus, lock_increase) {
  FlagStatus flagStatus;

  // Do nothing with ModifierFlag::NONE.
  flagStatus.lock_increase(ModifierFlag::NONE);
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());

  flagStatus.lock_increase(ModifierFlag::COMMAND_L);
  EXPECT_EQ(Flags(ModifierFlag::COMMAND_L), flagStatus.makeFlags());

  // lock don't cancel by reset & set.
  flagStatus.reset();
  flagStatus.set(KeyCode::A, Flags(0));
  EXPECT_EQ(Flags(ModifierFlag::COMMAND_L), flagStatus.makeFlags());

  flagStatus.lock_decrease(ModifierFlag::COMMAND_L);
  EXPECT_EQ(Flags(), flagStatus.makeFlags());
}

TEST(FlagStatus, negative_lock_increase) {
  FlagStatus flagStatus;

  // ----------------------------------------
  flagStatus.negative_lock_increase(ModifierFlag::COMMAND_L);
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());

  flagStatus.increase(ModifierFlag::COMMAND_L);
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());

  flagStatus.increase(ModifierFlag::COMMAND_L);
  EXPECT_EQ(Flags(ModifierFlag::COMMAND_L), flagStatus.makeFlags());

  // ----------------------------------------
  // lock don't cancel by reset & set.
  flagStatus.reset();
  flagStatus.set(KeyCode::A, Flags(0));
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());

  flagStatus.increase(ModifierFlag::COMMAND_L);
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());

  flagStatus.increase(ModifierFlag::COMMAND_L);
  EXPECT_EQ(Flags(ModifierFlag::COMMAND_L), flagStatus.makeFlags());

  // ----------------------------------------
  flagStatus.reset();
  flagStatus.negative_lock_decrease(ModifierFlag::COMMAND_L);
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());

  flagStatus.increase(ModifierFlag::COMMAND_L);
  EXPECT_EQ(Flags(ModifierFlag::COMMAND_L), flagStatus.makeFlags());
}

TEST(FlagStatus, lock_toggle) {
  FlagStatus flagStatus;

  flagStatus.lock_increase(ModifierFlag::COMMAND_L);
  EXPECT_EQ(Flags(ModifierFlag::COMMAND_L), flagStatus.makeFlags());

  flagStatus.lock_toggle(ModifierFlag::COMMAND_L);
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());

  flagStatus.lock_toggle(ModifierFlag::COMMAND_L);
  EXPECT_EQ(Flags(ModifierFlag::COMMAND_L), flagStatus.makeFlags());
}

TEST(FlagStatus, lock_clear) {
  FlagStatus flagStatus;

  {
    Vector_ModifierFlag v;
    v.push_back(ModifierFlag::COMMAND_L);
    v.push_back(ModifierFlag::FN);
    v.push_back(ModifierFlag::SHIFT_L);
    flagStatus.lock_increase(v);
    EXPECT_EQ(ModifierFlag::COMMAND_L | ModifierFlag::FN | ModifierFlag::SHIFT_L, flagStatus.makeFlags());
  }
  {
    flagStatus.increase(ModifierFlag::CAPSLOCK);
    EXPECT_EQ(ModifierFlag::CAPSLOCK | ModifierFlag::COMMAND_L | ModifierFlag::FN | ModifierFlag::SHIFT_L,
              flagStatus.makeFlags());
  }

  flagStatus.lock_clear();
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());
}

TEST(FlagStatus, negative_lock_clear) {
  FlagStatus flagStatus;

  {
    Vector_ModifierFlag v;
    v.push_back(ModifierFlag::COMMAND_L);
    v.push_back(ModifierFlag::FN);
    v.push_back(ModifierFlag::SHIFT_L);
    flagStatus.negative_lock_increase(v);
    EXPECT_EQ(Flags(0), flagStatus.makeFlags());

    flagStatus.increase(v);
    EXPECT_EQ(Flags(0), flagStatus.makeFlags());

    flagStatus.increase(v);
    EXPECT_EQ(ModifierFlag::COMMAND_L | ModifierFlag::FN | ModifierFlag::SHIFT_L, flagStatus.makeFlags());

    flagStatus.reset();
    EXPECT_EQ(Flags(0), flagStatus.makeFlags());

    flagStatus.increase(v);
    EXPECT_EQ(Flags(0), flagStatus.makeFlags());

    flagStatus.negative_lock_clear();
    EXPECT_EQ(ModifierFlag::COMMAND_L | ModifierFlag::FN | ModifierFlag::SHIFT_L, flagStatus.makeFlags());
  }
}

TEST(FlagStatus, sticky_increase) {
  FlagStatus flagStatus;

  // Do nothing with ModifierFlag::NONE.
  flagStatus.sticky_increase(ModifierFlag::NONE);
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());

  {
    Vector_ModifierFlag v;
    v.push_back(ModifierFlag::COMMAND_L);
    v.push_back(ModifierFlag::FN);
    flagStatus.sticky_increase(v);
    EXPECT_EQ(Flags(ModifierFlag::COMMAND_L | ModifierFlag::FN), flagStatus.makeFlags());
  }

  flagStatus.sticky_decrease(ModifierFlag::COMMAND_L);
  EXPECT_EQ(Flags(ModifierFlag::FN), flagStatus.makeFlags());
}

TEST(FlagStatus, sticky_toggle) {
  FlagStatus flagStatus;

  flagStatus.sticky_increase(ModifierFlag::COMMAND_L);
  EXPECT_EQ(Flags(ModifierFlag::COMMAND_L), flagStatus.makeFlags());

  flagStatus.sticky_toggle(ModifierFlag::COMMAND_L);
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());

  flagStatus.sticky_toggle(ModifierFlag::COMMAND_L);
  EXPECT_EQ(Flags(ModifierFlag::COMMAND_L), flagStatus.makeFlags());
}

TEST(FlagStatus, sticky_clear) {
  FlagStatus flagStatus;

  {
    Vector_ModifierFlag v;
    v.push_back(ModifierFlag::COMMAND_L);
    v.push_back(ModifierFlag::FN);
    v.push_back(ModifierFlag::SHIFT_L);
    flagStatus.sticky_increase(v);
    EXPECT_EQ(ModifierFlag::COMMAND_L | ModifierFlag::FN | ModifierFlag::SHIFT_L, flagStatus.makeFlags());
  }

  flagStatus.sticky_clear();
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());
}

TEST(FlagStatus, lazy_increase) {
  FlagStatus flagStatus;

  // +1 (total 1)
  flagStatus.lazy_increase(ModifierFlag::SHIFT_L);
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());

  // +0 (total 1)
  flagStatus.lazy_enable();
  EXPECT_EQ(Flags(ModifierFlag::SHIFT_L), flagStatus.makeFlags());

  // -1 (total 0)
  flagStatus.lazy_decrease(ModifierFlag::SHIFT_L);
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());

  // +1 (total 1)
  flagStatus.lazy_increase(ModifierFlag::SHIFT_L);
  EXPECT_EQ(Flags(ModifierFlag::SHIFT_L), flagStatus.makeFlags());

  flagStatus.lazy_disable_if_off();
  EXPECT_EQ(Flags(ModifierFlag::SHIFT_L), flagStatus.makeFlags());

  // +2 (total 2)
  flagStatus.lazy_increase(ModifierFlag::SHIFT_L);
  EXPECT_EQ(Flags(ModifierFlag::SHIFT_L), flagStatus.makeFlags());

  // -1 (total 1)
  flagStatus.lazy_decrease(ModifierFlag::SHIFT_L);
  EXPECT_EQ(Flags(ModifierFlag::SHIFT_L), flagStatus.makeFlags());

  // => 0 (lazy modifier is disabled when reset.)
  flagStatus.reset();

  // +1 (total 1)
  flagStatus.lazy_increase(ModifierFlag::SHIFT_L);
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());
}

TEST(FlagStatus, CapsLock) {
  FlagStatus flagStatus;

  flagStatus.set(KeyCode::CAPSLOCK, Flags(ModifierFlag::CAPSLOCK));
  EXPECT_EQ(Flags(ModifierFlag::CAPSLOCK), flagStatus.makeFlags());

  flagStatus.reset();

  flagStatus.set(KeyCode::A, Flags(ModifierFlag::CAPSLOCK));
  EXPECT_EQ(Flags(ModifierFlag::CAPSLOCK), flagStatus.makeFlags());

  // from other keyboard
  flagStatus.set(KeyCode::A, Flags(0));
  EXPECT_EQ(Flags(ModifierFlag::CAPSLOCK), flagStatus.makeFlags());

  flagStatus.set(KeyCode::A, Flags(ModifierFlag::CAPSLOCK));
  EXPECT_EQ(Flags(ModifierFlag::CAPSLOCK), flagStatus.makeFlags());

  // reset
  flagStatus.set(KeyCode::CAPSLOCK, Flags(0));
  EXPECT_EQ(Flags(), flagStatus.makeFlags());

  // soft caps
  flagStatus.lock_increase(ModifierFlag::CAPSLOCK);
  flagStatus.set(KeyCode::A, Flags(0));
  EXPECT_EQ(Flags(ModifierFlag::CAPSLOCK), flagStatus.makeFlags());

  // soft caps will be canceled by hardware caps
  flagStatus.set(KeyCode::CAPSLOCK, Flags(0));
  EXPECT_EQ(Flags(0), flagStatus.makeFlags());
}

TEST(FlagStatus, isOn) {
  {
    FlagStatus flagStatus;

    {
      Vector_ModifierFlag modifierFlags;
      EXPECT_TRUE(flagStatus.isOn(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag(ModifierFlag::ZERO));
      EXPECT_TRUE(flagStatus.isOn(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::NONE);
      EXPECT_TRUE(flagStatus.isOn(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::NONE);
      modifierFlags.push_back(ModifierFlag::ZERO);
      EXPECT_TRUE(flagStatus.isOn(modifierFlags));
    }
  }

  {
    FlagStatus flagStatus;
    flagStatus.increase(ModifierFlag::SHIFT_L);

    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::SHIFT_L);
      EXPECT_TRUE(flagStatus.isOn(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::SHIFT_L);
      modifierFlags.push_back(ModifierFlag::NONE);
      EXPECT_TRUE(flagStatus.isOn(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::SHIFT_R);
      EXPECT_FALSE(flagStatus.isOn(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::SHIFT_L);
      modifierFlags.push_back(ModifierFlag::ZERO);
      EXPECT_TRUE(flagStatus.isOn(modifierFlags));
    }
  }

  {
    FlagStatus flagStatus;
    flagStatus.increase(ModifierFlag::SHIFT_L);
    flagStatus.increase(ModifierFlag::ZERO);

    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::SHIFT_L);
      EXPECT_TRUE(flagStatus.isOn(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::SHIFT_L);
      modifierFlags.push_back(ModifierFlag::NONE);
      EXPECT_TRUE(flagStatus.isOn(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::SHIFT_R);
      EXPECT_FALSE(flagStatus.isOn(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::SHIFT_L);
      modifierFlags.push_back(ModifierFlag::ZERO);
      EXPECT_TRUE(flagStatus.isOn(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::SHIFT_L);
      modifierFlags.push_back(ModifierFlag::ZERO);
      modifierFlags.push_back(ModifierFlag::NONE);
      EXPECT_TRUE(flagStatus.isOn(modifierFlags));
    }
  }

  {
    FlagStatus flagStatus;
    flagStatus.increase(ModifierFlag::SHIFT_L);
    flagStatus.increase(ModifierFlag::CONTROL_R);
    flagStatus.increase(ModifierFlag::COMMAND_R);

    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::SHIFT_L);
      EXPECT_TRUE(flagStatus.isOn(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::SHIFT_L);
      modifierFlags.push_back(ModifierFlag::NONE);
      EXPECT_FALSE(flagStatus.isOn(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::SHIFT_R);
      EXPECT_FALSE(flagStatus.isOn(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::SHIFT_L);
      modifierFlags.push_back(ModifierFlag::CONTROL_R);
      modifierFlags.push_back(ModifierFlag::COMMAND_R);
      modifierFlags.push_back(ModifierFlag::NONE);
      EXPECT_TRUE(flagStatus.isOn(modifierFlags));
    }
  }
}

TEST(FlagStatus, isLocked) {
  {
    FlagStatus flagStatus;

    {
      Vector_ModifierFlag modifierFlags;
      EXPECT_TRUE(flagStatus.isLocked(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag(ModifierFlag::ZERO));
      EXPECT_TRUE(flagStatus.isLocked(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::NONE);
      EXPECT_TRUE(flagStatus.isLocked(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::NONE);
      modifierFlags.push_back(ModifierFlag::ZERO);
      EXPECT_TRUE(flagStatus.isLocked(modifierFlags));
    }

    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::SHIFT_L);
      EXPECT_FALSE(flagStatus.isLocked(modifierFlags));

      flagStatus.increase(ModifierFlag::SHIFT_L);
      EXPECT_FALSE(flagStatus.isLocked(modifierFlags));

      flagStatus.lock_increase(ModifierFlag::SHIFT_L);
      EXPECT_TRUE(flagStatus.isLocked(modifierFlags));
    }
  }
}

TEST(FlagStatus, isStuck) {
  {
    FlagStatus flagStatus;

    {
      Vector_ModifierFlag modifierFlags;
      EXPECT_TRUE(flagStatus.isStuck(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag(ModifierFlag::ZERO));
      EXPECT_TRUE(flagStatus.isStuck(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::NONE);
      EXPECT_TRUE(flagStatus.isStuck(modifierFlags));
    }
    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::NONE);
      modifierFlags.push_back(ModifierFlag::ZERO);
      EXPECT_TRUE(flagStatus.isStuck(modifierFlags));
    }

    {
      Vector_ModifierFlag modifierFlags;
      modifierFlags.push_back(ModifierFlag::SHIFT_L);
      EXPECT_FALSE(flagStatus.isStuck(modifierFlags));

      flagStatus.increase(ModifierFlag::SHIFT_L);
      EXPECT_FALSE(flagStatus.isStuck(modifierFlags));

      flagStatus.sticky_increase(ModifierFlag::SHIFT_L);
      EXPECT_TRUE(flagStatus.isStuck(modifierFlags));

      flagStatus.sticky_clear();
      EXPECT_FALSE(flagStatus.isStuck(modifierFlags));
    }
  }
}

TEST(FlagStatus, subtract) {
  FlagStatus flagStatus1;
  FlagStatus flagStatus2;

  flagStatus1.increase(ModifierFlag::CONTROL_L);
  flagStatus1.increase(ModifierFlag::OPTION_L);
  flagStatus1.increase(ModifierFlag::SHIFT_L);
  flagStatus1.increase(ModifierFlag::SHIFT_L);
  flagStatus1.decrease(ModifierFlag::COMMAND_R);

  flagStatus2.increase(ModifierFlag::CONTROL_L);
  flagStatus2.increase(ModifierFlag::FN);

  Vector_ModifierFlag v;
  flagStatus1.subtract(flagStatus2, v);
  EXPECT_EQ(3, v.size());
  EXPECT_EQ(ModifierFlag::OPTION_L, v[0]);
  EXPECT_EQ(ModifierFlag::SHIFT_L, v[1]);
  EXPECT_EQ(ModifierFlag::SHIFT_L, v[2]);

  flagStatus2.subtract(flagStatus1, v);
  EXPECT_EQ(2, v.size());
  EXPECT_EQ(ModifierFlag::COMMAND_R, v[0]);
  EXPECT_EQ(ModifierFlag::FN, v[1]);
}

TEST(FlagStatus, ScopedSetter) {
  FlagStatus flagStatus1;
  FlagStatus flagStatus2;

  flagStatus1.increase(ModifierFlag::CONTROL_L);
  flagStatus1.increase(ModifierFlag::OPTION_L);
  flagStatus1.increase(ModifierFlag::SHIFT_L);
  flagStatus1.increase(ModifierFlag::SHIFT_L);
  flagStatus1.decrease(ModifierFlag::COMMAND_R);

  flagStatus2.increase(ModifierFlag::COMMAND_R);
  flagStatus2.increase(ModifierFlag::CONTROL_L);
  flagStatus2.increase(ModifierFlag::FN);

  {
    EXPECT_EQ(flagStatus1.makeFlags(),
              ModifierFlag::CONTROL_L | ModifierFlag::OPTION_L | ModifierFlag::SHIFT_L);
    EXPECT_EQ(flagStatus2.makeFlags(),
              ModifierFlag::COMMAND_R | ModifierFlag::CONTROL_L | ModifierFlag::FN);

    {
      FlagStatus::ScopedSetter scopedSetter(flagStatus1, flagStatus2);

      EXPECT_EQ(flagStatus1.makeFlags(),
                ModifierFlag::COMMAND_R | ModifierFlag::CONTROL_L | ModifierFlag::FN);
      EXPECT_EQ(flagStatus2.makeFlags(),
                ModifierFlag::COMMAND_R | ModifierFlag::CONTROL_L | ModifierFlag::FN);
    }

    EXPECT_EQ(flagStatus1.makeFlags(),
              ModifierFlag::CONTROL_L | ModifierFlag::OPTION_L | ModifierFlag::SHIFT_L);
    EXPECT_EQ(flagStatus2.makeFlags(),
              ModifierFlag::COMMAND_R | ModifierFlag::CONTROL_L | ModifierFlag::FN);
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
