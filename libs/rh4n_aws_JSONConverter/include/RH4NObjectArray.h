#ifndef RH4N_AWS_JSONCONVERTER_RH4NOBJECTARRAY
#define RH4N_AWS_JSONCONVERTER_RH4NOBJECTARRAY

namespace RH4N::aws::JSONConverter {
    class RH4NObjectArray {
        private:
            Aws::Utils::Array<Aws::Utils::Json::JsonView> target;
            Signatures::ArraySignature *arrcontainer;
            Aws::Utils::Json::JsonView getFirstObject();
            bool compareAllSignatures(Signatures::ObjectSignature*, Aws::Utils::Array<Aws::Utils::Json::JsonView>, int);
        public:
            RH4NObjectArray(Aws::Utils::Array<Aws::Utils::Json::JsonView>, Signatures::ArraySignature*);
            Signatures::ObjectSignature *getSignature();
    };
}

#endif
