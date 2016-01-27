#include "GlobalLock.hpp"
#include "IOLogWrapper.hpp"

namespace org_pqrs_Karabiner {
IOLock* GlobalLock::lock_ = nullptr;

void GlobalLock::initialize(void) {
  lock_ = IOLockAlloc();
  if (!lock_) {
    IOLOG_ERROR("IOLockAlloc failed.\n");
  }
}

void GlobalLock::terminate(void) {
  if (!lock_) return;

  IOLockLock(lock_);
  IOLock* tmp = lock_;
  lock_ = nullptr;
  IOLockUnlock(tmp);

  // roughly sleep:
  IOSleep(200);

  IOLockFree(tmp);
}

// ------------------------------------------------------------
GlobalLock::ScopedLock::ScopedLock(void) {
  lock_ = GlobalLock::lock_;
  if (!lock_) return;

  IOLockLock(lock_);
}

GlobalLock::ScopedLock::~ScopedLock(void) {
  if (!lock_) return;

  IOLockUnlock(lock_);
}

bool GlobalLock::ScopedLock::operator!(void)const {
  return lock_ == nullptr;
}

// ------------------------------------------------------------
GlobalLock::ScopedUnlock::ScopedUnlock(void) {
  lock_ = GlobalLock::lock_;
  if (!lock_) return;

  IOLockUnlock(lock_);
}

GlobalLock::ScopedUnlock::~ScopedUnlock(void) {
  if (!lock_) return;

  IOLockLock(lock_);
}

bool GlobalLock::ScopedUnlock::operator!(void)const {
  return lock_ == nullptr;
}
}
