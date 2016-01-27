#include "pqrs/file_path.hpp"
#include "pqrs/vector.hpp"
#include <iostream>

namespace pqrs {
namespace file_path {
namespace {
size_t
get_dirname_position(const std::string& path, size_t pos = std::string::npos) {
  if (path.empty()) return 0;

  if (pos == std::string::npos) {
    pos = path.size() - 1;
  }

  if (path.size() <= pos) return 0;

  if (pos == 0) {
    // We retain the first slash for dirname("/") == "/".
    if (path[pos] == '/') {
      return 1;
    } else {
      return 0;
    }
  }

  if (path[pos] == '/') {
    --pos;
  }

  size_t i = path.rfind('/', pos);
  if (i == std::string::npos) {
    return 0;
  }
  if (i == 0) {
    // path starts with "/".
    return 1;
  }
  return i;
}

size_t
process_dot(const std::string& path, size_t pos) {
  if (path.empty()) return pos;

  // foo/bar/./
  //          ^
  //         pos
  //
  if (pos > 2 &&
      path[pos - 2] == '/' &&
      path[pos - 1] == '.') {
    return pos - 2;
  }

  // ./foo/bar
  //  ^
  //  pos
  //
  if (pos == 1 &&
      path[0] == '.') {
    return 0;
  }

  return pos;
}

size_t
process_dotdot(const std::string& path, size_t pos) {
  // Ignore ../../
  if (pos > 4 &&
      path[pos - 5] == '.' &&
      path[pos - 4] == '.' &&
      path[pos - 3] == '/' &&
      path[pos - 2] == '.' &&
      path[pos - 1] == '.') {
    return pos;
  }

  // foo/bar/../
  //           ^
  //          pos
  //
  if (pos > 2 &&
      path[pos - 3] == '/' &&
      path[pos - 2] == '.' &&
      path[pos - 1] == '.') {
    pos = get_dirname_position(path, pos - 3);

    // foo/bar/../
    //    ^
    //   pos

    return pos;
  }

  return pos;
}
}

std::string
dirname(const std::string& path) {
  size_t pos = get_dirname_position(path);
  if (pos == 0) {
    return ".";
  }
  return path.substr(0, pos);
}

void normalize(std::string& path) {
  if (path.empty()) {
    path += '.';
    return;
  }

  size_t end = path.size();
  size_t dest = 1;
  for (size_t src = 1; src < end; ++src) {
    // Skip multiple slashes.
    if (path[dest - 1] == '/' && path[src] == '/') {
      continue;
    }

    // Handling . and ..
    if (path[src] == '/') {
      dest = process_dot(path, dest);
      dest = process_dotdot(path, dest);

      if (dest == 0) {
        if (path[0] != '/') {
          continue;
        }
      } else {
        if (path[dest - 1] == '/') {
          continue;
        }
      }
    }

    if (dest != src) {
      path[dest] = path[src];
    }

    ++dest;
  }

  dest = process_dot(path, dest);
  dest = process_dotdot(path, dest);

  if (dest == 0) {
    path[0] = '.';
    dest = 1;
  }

  path.resize(dest);
}
}
}
