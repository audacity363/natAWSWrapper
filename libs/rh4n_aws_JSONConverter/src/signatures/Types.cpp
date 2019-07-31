#include <JSONConverter.h>

namespace RH4N::aws::JSONConverter::Signatures {
    Types *Types::getTypefromObject(Aws::Utils::Json::JsonView &var) {

        if(var.IsObject()) {
            return new Types(Types::OBJECT);
        } else if(var.IsBool()) {
            return new Types(Types::BOOLEAN);
        } else if(var.IsString()) {
            return new Types(Types::STRING);
        } else if(var.IsIntegerType()) {
            return new Types(Types::INT);
        } else if(var.IsFloatingPointType()) {
            return new Types(Types::FLOAT);
        } else if(var.IsListType()) {
            return new Types(Types::ARRAY);
        } else if(var.IsNull()) {
            return new Types(Types::NULLOBJECT);
        }

        return new Types(Types::UNKOWN);
    }


    std::ostream& operator << (std::ostream &stream, const Types &type) {
        switch(type) {
            case Types::STRING:
                stream << "STRING";
                break;
            case Types::BOOLEAN:
                stream << "BOOLEAN";
                break;
            case Types::OBJECT:
                stream << "OBJECT";
                break;
            case Types::INT:
                stream << "INT";
                break;
            case Types::FLOAT:
                stream << "FLOAT";
                break;
            case Types::ARRAY:
                stream << "ARRAY";
                break;
            case Types::NULLOBJECT:
                stream << "NULLOBJECT";
                break;
            default:
                stream << "UNKOWN";
        }

        return(stream);
    }
}
