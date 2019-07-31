#ifndef RH4N_AWS_JSONCONVERTER_SIGNATURES_TYPES
#define RH4N_AWS_JSONCONVERTER_SIGNATURES_TYPES

#include <aws/core/utils/json/JsonSerializer.h>

namespace RH4N::aws::JSONConverter::Signatures {
    class Types {
        public:
            enum Value {
                OBJECT = 0,
                STRING = 1,
                BOOLEAN = 2,
                INT = 3,
                FLOAT = 4,
                ARRAY = 5,
                NULLOBJECT = 6,
                UNKOWN = 7
            };

            Types() = default;
            Types(Types &source) {
                this->type = source.getType();
            }
            Types(Types::Value &source) {
                this->type = source;
            }
            constexpr Types(Value vartype) : type(vartype) {}

            operator Value() const { return type; }
            explicit operator bool() = delete;
            constexpr bool operator==(Types a) const { return type == a.type; }
            constexpr bool operator!=(Types a) const { return type != a.type; }
            constexpr bool operator==(Types::Value a) const { return type == a; }

            static Types *getTypefromObject(Aws::Utils::Json::JsonView&);
            int getNumberRep() const { return static_cast<int>(type); }
            Types::Value getType() { return(this->type); }
            friend std::ostream &operator << (std::ostream&, const Types&);
        private:
            Value type;
    };
}
#endif
