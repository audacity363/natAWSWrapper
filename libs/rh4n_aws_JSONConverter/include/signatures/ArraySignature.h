#ifndef RH4N_AWS_JSONCONVERTER_SIGNATURES_ARRAYSIGNATURE
#define RH4N_AWS_JSONCONVERTER_SIGNATURES_ARRAYSIGNATURE

namespace RH4N::aws::JSONConverter::Signatures {
    class ArraySignature {
        public:
            short dimensions = 1;
            int length[3] = {-1, -1, -1};
            Types *vartype = NULL;

            ArraySignature() { }
            ArraySignature(ArraySignature&);
            ArraySignature(ArraySignature*);
            ~ArraySignature();
            bool equals(ArraySignature*);
            friend std::ostream &operator << (std::ostream&, const ArraySignature&);
    };
}

#endif
