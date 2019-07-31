#include <JSONConverter.h>

namespace RH4N::aws::JSONConverter {
    RH4NObjectArray::RH4NObjectArray(Aws::Utils::Array<Aws::Utils::Json::JsonView> target, Signatures::ArraySignature *arrcontainer) {
        this->target = target;
        this->arrcontainer = arrcontainer;
    }

    Signatures::ObjectSignature *RH4NObjectArray::getSignature() {
        Signatures::ObjectSignature *objsig = NULL;
        Aws::Utils::Json::JsonView firstentry;

        firstentry = this->getFirstObject();
        objsig = RH4NObject(&firstentry).getSignature();

        if(!this->compareAllSignatures(objsig, this->target, 1)) {
            throw std::runtime_error("Every Object have to have the same signature");
        }

        objsig->convertToArray(this->arrcontainer);
        return(objsig);
    }

    Aws::Utils::Json::JsonView RH4NObjectArray::getFirstObject() {
        Aws::Utils::Array<Aws::Utils::Json::JsonView> target = this->target;
        Aws::Utils::Json::JsonView hptr;

        for(int i = 0; i < this->arrcontainer->dimensions; i++) {
            hptr = target[0];
            if(i+1 < this->arrcontainer->dimensions) {
                target = hptr.AsArray();
            }
        }

        return(hptr.AsObject());
    }

    bool RH4NObjectArray::compareAllSignatures(Signatures::ObjectSignature *rootsig, 
            Aws::Utils::Array<Aws::Utils::Json::JsonView> value, int curdim) {
        Signatures::ObjectSignature *tmpsig = NULL;

        for(int i = 0; i < value.GetLength(); i++) {
            Aws::Utils::Json::JsonView target = value[i];
            if(curdim != this->arrcontainer->dimensions) {
                if(!compareAllSignatures(rootsig, target.AsArray(), curdim+1)) {
                    return(false);
                }
                continue;
            }

            tmpsig = RH4NObject(&value[i]).getSignature();
            if(!rootsig->equals(tmpsig)) {
                delete tmpsig;
                return(false);
            }
            delete tmpsig;
        }
    }
}
