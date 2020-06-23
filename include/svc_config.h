#ifndef __SVC_CONFIG_H__
#define __SVC_CONFIG_H__

#include <string>

#define SVC_CONFIG 0

#define SVC_INSTALL_PATH "/usr/local/bin/svc"

namespace indem {

class SVCConfig final {
  SVCConfig() {}
  ~SVCConfig() {}
public:
  static std::string GetInstallPath() { return SVC_INSTALL_PATH; }
};

}

#endif /* __SVC_CONFIG_H__ */
