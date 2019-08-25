#include <JSONConverter.h>

namespace RH4N::aws::JSONConverter::Utils {
    void NameStack::pushName(std::string name) {
        this->stack.push_back(name);
    }

    std::string NameStack::popName() {
        std::string result = this->stack.back();
        this->stack.pop_back();
        return(result);
    }

    int NameStack::size() const {
        return(this->stack.size());
    }

    std::string NameStack::get(int index) const {
        return(this->stack.at(index));
    }

    std::ostream& operator << (std::ostream &stream, const NameStack &pointer) {
        int length = pointer.size();

        for(int i = 0; i < length; i++) {
            stream << pointer.get(i);
            if(i+1 < length) {
                stream << ".";
            }
        }

        return(stream);
    }

    const char **NameStack::AsArray() {
        int length = this->size();

        int sizeOfResult = sizeof(char*)*(length+1);

        const char **result = (const char**)(malloc(sizeOfResult));
        if(result == NULL) {
            return(NULL);
        }
        
        memset(result, 0x00, sizeOfResult);

        for(int i = 0; i < length; i++) {
            result[i] = this->stack[i].c_str();
        }
        return(result);
    }
}
