#ifndef RH4N_AWS_JSONCONVERTER_SIGNATURES_OBJECTSIGNATURE
#define RH4N_AWS_JSONCONVERTER_SIGNATURES_OBJECTSIGNATURE

namespace RH4N::aws::JSONConverter::Signatures {
    class ObjectSignature {
        private:
            ObjectSignatureNode *head = NULL;

            int index[3] = {0, 0, 0};
            int dim = 0;
            bool compareNode(ObjectSignatureNode*, 
                    RH4N::aws::JSONConverter::Utils::NameStack*, ObjectSignature*);
            void printEntry(ObjectSignatureNode*, int);
            void printTabs(int);
            ObjectSignatureNode *getLast();
            ObjectSignatureNode* getNode(ObjectSignatureNode*, std::string name);
            void fillValues(ObjectSignatureNode *hptr, RH4N::aws::JSONConverter::Utils::NameStack *pointer, 
                        Aws::Utils::Json::JsonView &root, RH4nVarList*);
            Aws::Utils::Json::JsonView getJsonTreeEntry(Aws::Utils::Json::JsonView&, RH4N::aws::JSONConverter::Utils::NameStack*);
            void fillArray(ObjectSignatureNode*, RH4N::aws::JSONConverter::Utils::NameStack*, int, int[3], Aws::Utils::Json::JsonView&, RH4nVarList*);
            void fillObjectArray(ObjectSignatureNode*, RH4N::aws::JSONConverter::Utils::NameStack*, int, int[3], Aws::Utils::Json::JsonView&, RH4nVarList*);
            int initRH4NVarListNode(ObjectSignatureNode*, RH4N::aws::JSONConverter::Utils::NameStack*, RH4nVarList*);
            void initRH4NVarListArray(ObjectSignatureNode*, const char**, RH4nVarList*);

        public:
            ~ObjectSignature();
            void freeList();
            ObjectSignatureNode* addAtEnd(std::string);
            ObjectSignatureNode* getHead();
            void printList();
            bool equals(ObjectSignature*);
            void convertToArray(ArraySignature*);
            void convertToArray(ObjectSignatureNode*, ArraySignature*);
            ObjectSignatureNode* getNode(RH4N::aws::JSONConverter::Utils::NameStack*);
            void fillValues(Aws::Utils::Json::JsonView &root, RH4nVarList*);
            RH4nVarList *initRH4NVarList(); 
    };
}

#endif
