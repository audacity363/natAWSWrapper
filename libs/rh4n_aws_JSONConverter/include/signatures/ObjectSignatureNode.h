#ifndef RH4N_AWS_JSONCONVERTER_SIGNATURES_OBJECTSIGNATURENODE 
#define RH4N_AWS_JSONCONVERTER_SIGNATURES_OBJECTSIGNATURENODE 

namespace RH4N::aws::JSONConverter::Signatures {
    class ObjectSignatureNode {
        public:
            std::string name;
            Types *vartype = NULL;
            Types *originalvartype = NULL;

            ArraySignature *arrsig = NULL;

            ArraySignature *originalarrsig = NULL;

            ObjectSignatureNode *next = NULL;
            ObjectSignatureNode *nextlvl = NULL;
            ObjectSignatureNode *prev = NULL;

            ~ObjectSignatureNode();
            ObjectSignatureNode();
            bool equals(ObjectSignatureNode*);
            friend std::ostream &operator << (std::ostream&, const ObjectSignatureNode&);
    };
}

#endif
