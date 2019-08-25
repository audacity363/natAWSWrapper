#include <iostream>
#include <cstring>

#include <JSONConverter.h>

using namespace RH4N::aws::JSONConverter::Utils;

namespace RH4N::aws::JSONConverter::Signatures {

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

    RH4nVarList *ObjectSignature::initRH4NVarList() {
        RH4nVarList *varlist = (RH4nVarList*)malloc(sizeof(RH4nVarList));
        if(varlist == NULL) {
            throw new std::runtime_error("Could not allocate RH4nVarList");
        }

        NameStack *pointer = new NameStack();

        rh4nvarInitList(varlist);

        this->initRH4NVarListNode(this->head, pointer, varlist);

        delete(pointer);
    
        return(varlist);
    }

    int ObjectSignature::initRH4NVarListNode(ObjectSignatureNode *hptr, NameStack *pointer, RH4nVarList *varlist) {
        int rh4nvarret = 0;
        const char **parents = NULL;
        const char *chptr;

        if((parents = pointer->AsArray()) == NULL) {
            throw new std::runtime_error("NameStack->AsArray returned NULL");
        }

        for(; hptr != NULL; hptr = hptr->next) {
            if(hptr->nextlvl) {
                if((rh4nvarret = rh4nvarCreateNewGroup_m(varlist, (char**)parents, (char*)hptr->name.c_str())) != RH4N_RET_OK) {
                    throw std::runtime_error("rh4nvarCreateNewGroup_m: Error in RH4nVar Libarary: " + std::to_string(rh4nvarret));
                    free(parents);
                }

                pointer->pushName(hptr->name);
                if((rh4nvarret = this->initRH4NVarListNode(hptr->nextlvl, pointer, varlist)) != RH4N_RET_OK) {
                    free(parents);
                    throw std::runtime_error("Error in RH4nVar Libarary: " + std::to_string(rh4nvarret));
                }
                pointer->popName();
                //It seems that the intenal array from the vector in the NameStack gets realocated
                free(parents);
                parents = pointer->AsArray();
                continue;
            }

            if(*hptr->vartype == Types::NULLOBJECT) {
                continue;
            }

            switch(*hptr->vartype) {
                case Types::INT:
                    rh4nvarCreateNewInt_m(varlist, (char**)parents, (char*)hptr->name.c_str(), 0);
                    break;
                case Types::STRING:
                    rh4nvarCreateNewString_m(varlist, (char**)parents, (char*)hptr->name.c_str(), "");
                    break;
                case Types::FLOAT:
                    rh4nvarCreateNewFloat_m(varlist, (char**)parents, (char*)hptr->name.c_str(), 0.0);
                    break;
                case Types::BOOLEAN:
                    rh4nvarCreateNewBool_m(varlist, (char**)parents, (char*)hptr->name.c_str(), false);
                    break;
                case Types::ARRAY:
                    this->initRH4NVarListArray(hptr, parents, varlist);
            }
        }

        free(parents);
        return(0);
    }

    void ObjectSignature::initRH4NVarListArray(ObjectSignatureNode *target, const char **parents, RH4nVarList *varlist) {
        switch(*target->arrsig->vartype) {
            case Types::INT:
                rh4nvarCreateNewIntArray_m(varlist, (char**)parents, (char*)target->name.c_str(), target->arrsig->dimensions, target->arrsig->length);
                break;
            case Types::STRING:
                rh4nvarCreateNewStringArray_m(varlist, (char**)parents, (char*)target->name.c_str(), target->arrsig->dimensions, target->arrsig->length);
                break;
            case Types::FLOAT:
                rh4nvarCreateNewFloatArray_m(varlist, (char**)parents, (char*)target->name.c_str(), target->arrsig->dimensions, target->arrsig->length);
                break;
            case Types::BOOLEAN:
                rh4nvarCreateNewBoolArray_m(varlist, (char**)parents, (char*)target->name.c_str(), target->arrsig->dimensions, target->arrsig->length);
                break;
        }
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

    using namespace Aws::Utils::Json;

    void ObjectSignature::fillValues(JsonView &root, RH4nVarList *varlist) {
        NameStack *pointer = new NameStack();

        this->fillValues(this->head, pointer, root, varlist);
    }

    void ObjectSignature::fillValues(ObjectSignatureNode *hptr, NameStack *pointer, JsonView &root, RH4nVarList *varlist) {
        JsonView target;
        int arrindex[3] = {0, 0, 0}, rh4nvarret = 0;
        const char **parents = NULL;
        RH4nVarRef ref = RH4NVAR_REF_INIT;

        for(; hptr != NULL; hptr = hptr->next) {
            pointer->pushName(hptr->name);
            
            if(*hptr->originalvartype == Types::OBJECT) {
                this->fillValues(hptr->nextlvl, pointer, root, varlist);
            } else if(*hptr->originalvartype == Types::ARRAY && *hptr->originalarrsig->vartype == Types::OBJECT) {
                this->fillObjectArray(hptr->nextlvl, pointer, 1, arrindex, root, varlist);
            } else if(*hptr->originalvartype == Types::ARRAY) {
                this->fillArray(hptr, pointer, 1, arrindex, root, varlist);
            } else {
                if(!(*hptr->vartype == Types::NULLOBJECT)) {
                    target = this->getJsonTreeEntry(root, pointer);
                    parents = pointer->AsArray();
                    parents[pointer->size()-1] = 0x00;
                    std::cout << "JSON Entry: " << *pointer << std::endl;
                    std::cout << "Parents: " << parents << std::endl;

                    if((rh4nvarret = rh4nvarGetRef_m(varlist, (char**)parents, (char*)hptr->name.c_str(), &ref)) != RH4N_RET_OK) {
                        throw std::runtime_error("rh4nvarGetRef_m retunred: " + std::to_string(rh4nvarret));
                    }

                    if(this->dim == 0) {
                        switch(*hptr->vartype) {
                            case Types::INT:
                                rh4nvarSetInt(&ref.var->var, target.AsInteger());
                                break;
                            case Types::STRING:
                                rh4nvarSetString(&ref.var->var, (char*)target.AsString().c_str());
                                break;
                            case Types::FLOAT:
                                rh4nvarSetFloat(&ref.var->var, target.AsDouble());
                                break;
                            case Types::BOOLEAN:
                                rh4nvarSetBool(&ref.var->var, target.AsBool());
                                break;
                        }
                    } else {
                        switch(*hptr->arrsig->vartype) {
                            case Types::INT:
                                rh4nvarSetIntArrayEntry_m(varlist, parents, hptr->name.c_str(), this->index, target.AsInteger());
                                break;
                            case Types::STRING:
                                rh4nvarSetStringArrayEntry_m(varlist, parents, hptr->name.c_str(), this->index, (char*)target.AsString().c_str());
                                break;
                            case Types::FLOAT:
                                rh4nvarSetFloatArrayEntry_m(varlist, parents, hptr->name.c_str(), this->index, target.AsDouble());
                                break;
                            case Types::BOOLEAN:
                                rh4nvarSetBoolArrayEntry_m(varlist, parents, hptr->name.c_str(), this->index, target.AsBool());
                                break;
                        }
                    }

                    free(parents);
                }
            }

            pointer->popName();
        }
    }

    void ObjectSignature::fillArray(ObjectSignatureNode *target, NameStack *pointer, int dim, int index[3], JsonView &root, RH4nVarList *varlist) {
        JsonView value;
        const char **parents = NULL;

        this->dim++;
        for(index[dim-1] = 0; index[dim-1] < target->originalarrsig->length[dim-1]; index[dim-1]++) {
            this->index[this->dim-1] = index[dim-1];

            if(dim < target->originalarrsig->dimensions) {
                this->fillArray(target, pointer, dim+1, index, root, varlist);
            } else {
                value = this->getJsonTreeEntry(root, pointer);

                parents = pointer->AsArray();
                parents[pointer->size()-1] = 0x00;

                std::cout << "Name: " << *pointer << std::endl;
                std::cout << "RH4N index: " << index[0] << ", " << index[1] << ", " << index[2] << std::endl;

                switch(*target->arrsig->vartype) {
                    case Types::INT:
                        rh4nvarSetIntArrayEntry_m(varlist, parents, target->name.c_str(), index, value.AsInteger());
                        break;
                    case Types::STRING:
                        rh4nvarSetStringArrayEntry_m(varlist, parents, target->name.c_str(), index, (char*)value.AsString().c_str());
                        break;
                    case Types::FLOAT:
                        rh4nvarSetFloatArrayEntry_m(varlist, parents, target->name.c_str(), index, value.AsDouble());
                        break;
                    case Types::BOOLEAN:
                        rh4nvarSetBoolArrayEntry_m(varlist, parents, target->name.c_str(), index, value.AsBool());
                        break;
                }
                free(parents);
            }
        }

        this->index[this->dim-1] = 0;
        this->dim--;
    }

    void ObjectSignature::fillObjectArray(ObjectSignatureNode *target, NameStack *pointer, int dim, int index[3], JsonView &root, RH4nVarList *varlist) {
        this->dim++;
        for(index[dim-1] = 0; index[dim-1] < target->originalarrsig->length[dim-1]; index[dim-1]++) {
            this->index[this->dim-1] = index[dim-1];
            
            if(dim < target->originalarrsig->length[dim-1]) {
                this->fillObjectArray(target, pointer, dim+1, index, root, varlist);
            } else {
                this->fillValues(target->nextlvl, pointer, root, varlist);
            }
        }
        this->index[this->dim-1] = 0;
        this->dim--;
    }

    JsonView ObjectSignature::getJsonTreeEntry(JsonView &root, NameStack *pointer) {
        JsonView target = root;

        for(int i = 0; i < pointer->size(); i++) {
            target = target.GetObject(Aws::String(pointer->get(i).c_str()));
        }

        return(target);
    }
}
