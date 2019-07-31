#include <iostream>
#include <stdexcept>

#include <JSONConverter.h>

namespace RH4N::aws::JSONConverter {
    RH4NArray::RH4NArray(Aws::Utils::Array<Aws::Utils::Json::JsonView> &target) {
        this->target = target;
    }

    Signatures::ArraySignature *RH4NArray::getSignature() {
        Signatures::ArraySignature *arrsig = new Signatures::ArraySignature();
        this->_getSignature(this->target, arrsig, 1);
        return(arrsig);
    }

    void RH4NArray::_getSignature(Aws::Utils::Array<Aws::Utils::Json::JsonView> value, Signatures::ArraySignature *sig, int dim) {
        Aws::Utils::Json::JsonView target;
        Signatures::Types *vartype = NULL;

        if(dim > 3) {
            throw std::runtime_error("More then three dimensions are not supported by natural");
        }

        sig->dimensions = dim;

        int length = value.GetLength();
        if(sig->length[sig->dimensions-1] < length) {
            sig->length[sig->dimensions-1] = length;
        }

        for(int i = 0; i < length; i++) {
            target = value[i];
            if(target.IsListType()) {
                _getSignature(target.AsArray(), sig, dim+1);
                continue;
            } 
            vartype = Signatures::Types::getTypefromObject(target);
            if(sig->vartype == NULL) {
                sig->vartype = vartype;
            } else if(*sig->vartype != *vartype) {
                delete vartype;
                throw new std::runtime_error("Found multiple vartypes in array");
            } else {
                delete vartype;
            }
        }
    }
}
