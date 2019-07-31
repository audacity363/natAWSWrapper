#include <iostream>
#include <cstring>

#include <JSONConverter.h>

using namespace RH4N::aws::JSONConverter::Utils;

namespace RH4N::aws::JSONConverter::Signatures {

    //Public

    ObjectSignature::~ObjectSignature() {
        //if(this->head) delete this->head;
    }

    ObjectSignatureNode *ObjectSignature::addAtEnd(std::string name) {
        ObjectSignatureNode *hptr = this->getLast();
        ObjectSignatureNode *newNode = new ObjectSignatureNode();

        if(hptr) {
            hptr->next = newNode;
        } else {
            this->head = newNode;
        }

        newNode->name = name;
        return(newNode);
    }

    ObjectSignatureNode *ObjectSignature::getHead() {
        return(this->head);
    }


    void ObjectSignature::freeList() {
        if(this->head) delete this->head;
    }

    void ObjectSignature::printList() {
        this->printEntry(this->head, 0);
    }

    bool ObjectSignature::equals(ObjectSignature *target) {
        NameStack *tmpNS = new NameStack();

        bool result = this->compareNode(this->head, tmpNS, target);
        delete tmpNS;

        return(result);
    }

    void ObjectSignature::convertToArray(ArraySignature *arrsig) {
        this->convertToArray(this->head, arrsig);
    }

    void ObjectSignature::convertToArray(ObjectSignatureNode *head, ArraySignature *arrsig) {
        ObjectSignatureNode *hptr = this->head;

        for(; hptr != NULL; hptr = hptr->next) {
            if(*hptr->vartype == Types::OBJECT) {
                this->convertToArray(hptr->nextlvl, arrsig);
            } else if(*hptr->vartype == Types::ARRAY) {
                if(hptr->arrsig->dimensions + arrsig->dimensions > 3) {
                    throw std::runtime_error("More than three dimensions are not suppoerted by natural");
                }
                std::memmove(hptr->arrsig->length + arrsig->dimensions, hptr->arrsig->length, sizeof(int)*(hptr->arrsig->dimensions));
                std::memcpy(hptr->arrsig->length, arrsig->length, sizeof(int)*(arrsig->dimensions));
                hptr->arrsig->dimensions += arrsig->dimensions;
            } else {
                hptr->arrsig = new ArraySignature(*arrsig);
                hptr->arrsig->vartype = new Types(*hptr->vartype);
                hptr->vartype = new Types(Types::ARRAY);
            }
        }
    }

    ObjectSignatureNode *ObjectSignature::getNode(NameStack *names) {
        ObjectSignatureNode *hptr = this->head;

        for(int i = 0; i < names->size(); i++) {
            hptr = this->getNode(hptr, names->get(i));
            if(!hptr) {
                return(NULL);
            }

            if(i+1 < names->size()) {
                hptr = hptr->nextlvl;
            }
        }
    }

    //Private
   
    bool ObjectSignature::compareNode(ObjectSignatureNode *target, NameStack *parents, 
            ObjectSignature *comparesig) {
        ObjectSignatureNode *hptr = target;
        ObjectSignatureNode *compareNode = NULL;

        for(; hptr != NULL; hptr = hptr->next) {
            parents->pushName(hptr->name);

            compareNode = comparesig->getNode(parents);
            if(!compareNode) {
                return(false);
            }
    
            if(!hptr->equals(compareNode)) {
                return(false);
            }

            if(hptr->nextlvl != NULL) {
                if(!this->compareNode(hptr->nextlvl, parents, comparesig)) {
                    return(false);
                }
            }

            parents->popName();
        }
    }

    void ObjectSignature::printEntry(ObjectSignatureNode *target, int level) {
        ObjectSignatureNode *hptr = NULL;
        hptr = target;

        while(hptr != NULL) {
            this->printTabs(level);
            std::cout <<  *hptr << std::endl;

            if(hptr->nextlvl) {
                this->printEntry(hptr->nextlvl, level+1);
            }

            hptr = hptr->next;
        }
    }

    void ObjectSignature::printTabs(int level) {
        for(int i = 0; i < level; i++) {
            std::cout << "\t";
        }
    }

    ObjectSignatureNode *ObjectSignature::getLast() {
        ObjectSignatureNode *hptr = this->head;
        if(!hptr) { return(NULL); }

        for(; hptr->next != NULL; hptr = hptr->next);
        return(hptr);
    }

    ObjectSignatureNode *ObjectSignature::getNode(ObjectSignatureNode *head, std::string name) {
        ObjectSignatureNode *hptr = head;

        for(; hptr != NULL; hptr = hptr->next) {
            if(hptr->name.compare(name) == 0) {
                return(hptr);
            }
        }

        return(NULL);
    }
}
