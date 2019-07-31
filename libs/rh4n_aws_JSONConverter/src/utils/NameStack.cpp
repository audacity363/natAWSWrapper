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

    int NameStack::size() {
        return(this->stack.size());
    }

    std::string NameStack::get(int index) {
        return(this->stack.at(index));
    }
}
