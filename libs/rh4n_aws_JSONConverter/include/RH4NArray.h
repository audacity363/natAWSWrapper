#ifndef RH4N_AWS_JSONCONVERTER_RH4NARRAY
#define RH4N_AWS_JSONCONVERTER_RH4NARRAY

#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/utils/Array.h>

namespace RH4N::aws::JSONConverter {
    class RH4NArray {
        private:
            Aws::Utils::Array<Aws::Utils::Json::JsonView> target;
            void _getSignature(Aws::Utils::Array<Aws::Utils::Json::JsonView>, Signatures::ArraySignature*, int);

        public:
            RH4NArray(Aws::Utils::Array<Aws::Utils::Json::JsonView>&);
            Signatures::ArraySignature *getSignature();
    };
}

#endif
