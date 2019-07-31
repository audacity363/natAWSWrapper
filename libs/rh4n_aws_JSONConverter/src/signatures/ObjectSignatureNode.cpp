#include <iostream>
#include <ostream>
#include <cstring>

#include <JSONConverter.h>

namespace RH4N::aws::JSONConverter::Signatures {

    ObjectSignatureNode::ObjectSignatureNode() {
        this->vartype = NULL;
        this->originalvartype = NULL;
        this->next = NULL;
        this->nextlvl = NULL;
        this->prev = NULL;
    }
    
    ObjectSignatureNode::~ObjectSignatureNode() {
        if(this->vartype != NULL) { 
            delete(this->vartype); 
            this->vartype = NULL;
        }

        if(this->originalvartype != NULL) {
            delete this->originalvartype;
            this->originalvartype = NULL;
        }

        if(this->arrsig != NULL) {
            delete this->arrsig;
            this->arrsig = NULL;
        }

        if(this->originalarrsig != NULL) {
            delete this->originalarrsig;
            this->originalvartype = NULL;
        }

        if(this->nextlvl != NULL) {
            delete this->nextlvl;
            this->nextlvl = NULL;
        }

        if(this->next != NULL) {
            delete this->next;
            this->next = NULL;
        }
    }

    bool ObjectSignatureNode::equals(ObjectSignatureNode *target) {
        if(this->name.compare(target->name) == -1) {
            return(false);
        } else if (this->vartype != target->vartype) {
            return(false);
        } else if(this->arrsig != NULL && target->arrsig != NULL) {
            return(false);
        } else if(this->arrsig == NULL && target->arrsig != NULL) {
            return(false);
        } else if(this->arrsig != NULL && target->arrsig != NULL) {
            if(!this->arrsig->equals(target->arrsig)) {
                return(false);
            }
        }
        return(true);
    }

    std::ostream& operator << (std::ostream &stream, const ObjectSignatureNode &osn) {
        stream << "Name: " << osn.name << " type: " << *osn.vartype;

        if(osn.arrsig) {
            stream << " Array: " << *osn.arrsig;
        }

        stream << " (Original: vartype: ";
        if(osn.originalvartype) { stream << *osn.originalvartype; } 
        else { stream << "null"; }

        stream << ", Array: ";
        if(osn.originalarrsig) { stream << *osn.originalarrsig; }
        else { stream << "null"; }

        return stream << ")";
    }
}
