#include "List.hpp"

namespace org_pqrs_Karabiner {
List::Item*
List::erase(Item* p) {
  if (!p) return nullptr;

  Item* next = p->next_;

  if (p->prev_) {
    p->prev_->next_ = p->next_;
  }
  if (p->next_) {
    p->next_->prev_ = p->prev_;
  }
  if (front_ == p) {
    front_ = p->next_;
  }
  if (back_ == p) {
    back_ = p->prev_;
  }

  --size_;

  return next;
}

List::Item*
List::erase_and_delete(Item* p) {
  if (!p) return nullptr;

  Item* next = erase(p);
  delete p;
  return next;
}

void List::clear(void) {
  while (front_) {
    erase_and_delete(front_);
  }
}

List::Item*
List::insert(Item* p, Item* newval) {
  if (!newval) return nullptr;

  if (p == nullptr) {
    // push front if p == nullptr

    if (front_) {
      front_->prev_ = newval;
    }

    newval->prev_ = nullptr;
    newval->next_ = front_;

    front_ = newval;

    if (!back_) {
      back_ = front_;
    }

  } else {
    newval->next_ = p;

    if (p->prev_) {
      newval->prev_ = p->prev_;

      p->prev_->next_ = newval;
    } else {
      front_ = newval;
    }

    p->prev_ = newval;
  }

  ++size_;
  return newval;
}

void List::push_back(Item* p) {
  if (!p) return;

  if (back_) {
    back_->next_ = p;
  }
  p->prev_ = back_;
  p->next_ = nullptr;

  back_ = p;

  if (!front_) {
    front_ = back_;
  }

  ++size_;
}

void List::push_front(Item* p) {
  if (!p) return;
  insert(nullptr, p);
}
}
