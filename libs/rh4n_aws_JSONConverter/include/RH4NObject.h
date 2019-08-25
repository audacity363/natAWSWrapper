#ifndef RH4N_AWS_JSONCONVERTER_RH4NOBJECT
#define RH4N_AWS_JSONCONVERTER_RH4NOBJECT

#include <aws/core/utils/json/JsonSerializer.h>

namespace RH4N::aws::JSONConverter {
    class RH4NObject {
        private:
            Aws::Map<Aws::String, Aws::Utils::Json::JsonView> target;
        public:
            RH4NObject(Aws::Utils::Json::JsonView*);
            RH4NObject(Aws::Map<Aws::String, Aws::Utils::Json::JsonView>);
            Signatures::ObjectSignature* getSignature();
    };
}

#endif
