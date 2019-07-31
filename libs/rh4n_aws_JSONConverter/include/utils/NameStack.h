#ifndef RH4N_AWS_JSONCONVERTER_UTILS_NAMESTACK
#define RH4N_AWS_JSONCONVERTER_UTILS_NAMESTACK

#include <vector>

namespace RH4N::aws::JSONConverter::Utils {
    class NameStack {
        private: 
            std::vector<std::string> stack;

        public:
            void pushName(std::string);
            std::string popName();
            std::string get(int);
            int size();
    };
}

#endif
