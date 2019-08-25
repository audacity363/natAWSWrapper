#include <JSONConverter.h>


namespace RH4N::aws::JSONConverter {
    RH4NObject::RH4NObject(Aws::Utils::Json::JsonView *target) {
        this->target = target->GetAllObjects();
    }

    RH4NObject::RH4NObject(Aws::Map<Aws::String, Aws::Utils::Json::JsonView> target) {
        this->target = target;
    }
    
    Signatures::ObjectSignature *RH4NObject::getSignature() {
        Signatures::ObjectSignature *objsig = new Signatures::ObjectSignature();
        Aws::String key;

        Signatures::ArraySignature *arr = NULL;
        Signatures::ObjectSignatureNode *newNode = NULL;

        for(std::map<Aws::String, Aws::Utils::Json::JsonView>::iterator objit = this->target.begin(); 
                    objit != this->target.end(); ++objit) {
            key = objit->first;
            Aws::Utils::Json::JsonView value = objit->second;

            newNode = objsig->addAtEnd(std::string(key.c_str(), key.size()));
            newNode->originalvartype = Signatures::Types::getTypefromObject(value);
            newNode->vartype = new Signatures::Types(*newNode->originalvartype);
            if(*newNode->vartype == Signatures::Types::ARRAY) {
                Aws::Utils::Array<Aws::Utils::Json::JsonView> tmparr = value.AsArray();
                newNode->arrsig = RH4NArray(tmparr).getSignature();
                newNode->originalarrsig = new Signatures::ArraySignature(newNode->arrsig);

                if(*newNode->arrsig->vartype == Signatures::Types::OBJECT) {
                    RH4NObjectArray objarrHandler(value.AsArray(), newNode->arrsig);

                    newNode->nextlvl = objarrHandler.getSignature()->getHead();
                    delete newNode->arrsig;
                    newNode->arrsig = NULL;

                    newNode->vartype == new Signatures::Types(Signatures::Types::OBJECT);
                }
            } else if(*newNode->vartype == Signatures::Types::OBJECT) {
                RH4NObject obj(&value);
                Signatures::ObjectSignature *tmpsig = obj.getSignature();
                newNode->nextlvl = tmpsig->getHead();
                delete tmpsig;
            }
        }
        return(objsig);
    }
}
