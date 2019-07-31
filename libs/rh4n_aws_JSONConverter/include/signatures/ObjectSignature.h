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
    };
}

#endif
