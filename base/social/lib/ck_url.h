#ifndef URL_H
#define URL_H

#include <social_kit_export.h>

namespace social_kit {
class DECL_SOCIAL_KIT_EXPORT url
{
public:
    url();
    virtual ~url();
private:
    class platform_url_handle;
    platform_url_handle * const ctx;
};
}

#endif // URL_H
