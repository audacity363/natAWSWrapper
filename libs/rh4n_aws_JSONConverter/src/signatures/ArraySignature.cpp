#include <iostream>
#include <ostream>
#include <cstring>

#include <JSONConverter.h>

namespace RH4N::aws::JSONConverter::Signatures {
    ArraySignature::ArraySignature(ArraySignature &source) {
        this->dimensions = source.dimensions;
        std::memcpy(this->length, source.length, sizeof(int)*3);
        this->vartype = new Types(*source.vartype);
    }

    ArraySignature::ArraySignature(ArraySignature *source) {
        this->dimensions = source->dimensions;
        std::memcpy(this->length, source->length, sizeof(int)*3);
        this->vartype = new Types(*source->vartype);
    }

    ArraySignature::~ArraySignature() {
        if(this->vartype) delete this->vartype;
    }

    bool ArraySignature::equals(ArraySignature *target) {
        if(this->dimensions != target->dimensions) {
            return(false);
        } else if(std::memcmp(this->length, target->length, sizeof(int)*3) != 0) {
            return(false);
        } else if(this->vartype != target->vartype) {
            return(false);
        }

        return(true);
    }

    std::ostream& operator << (std::ostream &stream, const ArraySignature &as) {
        return stream << "ArraySignature(Type: [" << *as.vartype << "] Dimensions: [" \
            << as.dimensions << "] Length: [" << as.length[0] \
            << ", " << as.length[1] << ", " << as.length[2] << "])";
    }
}
