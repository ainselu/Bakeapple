#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <map>
#include <unordered_map>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <cstdarg>
#include <cstring>
#include <sstream>
#include <dlfcn.h>

namespace bakeapple_err {
    class invalidTypeError : public std::exception {
        public:
            const char* what() const noexcept override {
                return "\033[97m[\033[91mERROR\033[97m] | Invalid type specified in return keyword\033[0m\n";
            }
    };

    class nilValueError : public std::exception {
        public:
            const char* what() const noexcept override {
                return "\033[97m[\033[91mERROR\033[97m] | Nil type cannot contain a value other than zero\033[0m\n";
            }
    };

    class booleanValueError : public std::exception {
        public:
            const char* what() const noexcept override {
                return "\033[97m[\033[91mERROR\033[97m] | Boolean type cannot contain a value other than true or false\033[0m\n";
            }
    };

    class characterValueError : public std::exception {
        public:
            const char* what() const noexcept override {
                return "\033[97m[\033[91mERROR\033[97m] | Character type must be a character\033[0m\n";
            }
    };

    class stringFunctionError : public std::exception {
        public:
            const char* what() const noexcept override {
                return "\033[97m[\033[91mERROR\033[97m] | Attempted to load string value of register holding a noncorrespondent type\033[0m\n";
            }
    };

    class syntaxError : public std::exception {
        public:
            const char* what() const noexcept override {
                return "\033[97m[\033[91mERROR\033[97m] | Syntax error\033[0m\n";
            }
    };

    class unknownInstructError : public std::exception {
        public:
            const char* what() const noexcept override {
                return "\033[97m[\033[91mERROR\033[97m] | Unknown instruction\033[0m\n";
            }
    };

    class incorrectReturnTypeError : public std::exception {
        public:
            const char* what() const noexcept override {
                return "\033[97m[\033[91mERROR\033[97m] | Attempted to return disallowed type\033[0m\n";
            }
    };

    class invalidRegisterError : public std::exception {
        public:
            const char* what() const noexcept override {
                return "\033[97m[\033[91mERROR\033[97m] | Attempted to modify the value of a register which was not found or no longer exists\033[0m\n";
            }
    };
}

namespace bakeapple_tkn {
    std::vector<std::string> tokenize(std::string s, char delimiter = ' ') {
        std::vector<std::string> r1;
        std::string r;
        bool quote = false;
        bool braces = false;

        for (size_t i = 0; i < s.length(); i++) {
            if (!quote && !braces) {
                if (s[i] != delimiter && s[i] != '\"' && s[i] != '{') { 
                    r.push_back(s[i]);
                } else if (s[i] == '\"') {
                    quote = true;
                } else if (s[i] == '{') {
                    braces = true;
                } else if (!r.empty()) { 
                    r1.push_back(r);
                    r.clear();
                }
            } else if (quote) {
                if (s[i] == '\"') {
                    r1.push_back(r);
                    r.clear();
                    quote = false;
                } else {
                    r.push_back(s[i]);
                }
            } else if (braces) {
                if (s[i] == '}') {
                    r1.push_back(r);
                    r.clear();
                    braces = false;
                } else {
                    r.push_back(s[i]);
                }
            }
        }

        if (!r.empty()) {
            r1.push_back(r);
        }
        
        return r1;
    }
}

typedef enum types {
    integer = 0x696E74,
    string = 0x0A737472,
    character = 0x63686172,
    boolean = 0x626F6F6C,
    nil = 0x6E696C
} types;

std::unordered_map<std::string, enum types> typeconv = {
    {"integer", integer},
    {"string", string},
    {"character", character},
    {"boolean", boolean},
    {"nil", nil}
};

class reg {
    public:
        uint64_t strg = 0;
        enum types type = integer; // stores what type tue value in the register should be REGIsTERED as.,., hahahe imj so fukcign fucnnny oh mungkdsas jdjkabhk   n.kjdqwdj

        int store(std::string *v, enum types t){
            switch (t){
                case integer:
                    strg = std::stoi((*v));
                    type = integer;
                    return 0;
                    break;

                case nil:
                    strg=0;
                    type=nil;
                    return 0;
                    break;

                case string: {
                    std::string *s = new std::string((*v));
                    strg=(uint32_t)(uintptr_t)s;
                    type=string;
                    return 0;
                    break;
                }
                case boolean:
                    if ((*v)=="true"){
                        strg=1;
                        type=boolean;
                        return 0;
                        break;
                    } else if ((*v)=="false") {
                        strg=0;
                        type=boolean;
                        return 0;
                        break;
                    } else {
                        throw bakeapple_err::booleanValueError();
                    }
                    break;

                case character:
                    if (v->length()==1){
                        type=character;
                        char c = (*v)[0];
                        strg=c;
                        return 0;
                        break;
                    }
                    
            }
            return 0;
        }

        std::string ldrstr(){
            if (type=string){
                std::string *rt = reinterpret_cast<std::string*>(strg);
                return (*rt);
            } else {
                throw bakeapple_err::stringFunctionError();
            }
        }

        std::string ldrbool(){
            switch (type){
            case string:
                return "true"; // always
                break;
            
            case !string:
                if (strg==0){
                    return "false";
                } else if (strg==1) {
                    return "true";
                }

            default:
                break;
            }
        }
};

reg x0, x1, x2, x3, x4,
    x5, x6, x7, x8, x9; // 15 general purpose registers, should be enough

reg x10, x11, x12, x13, x14;

reg sy; // Last operation return value
reg rv; // Return value of last called label

std::unordered_map<std::string, reg*> regconv = {
    {"x0", &x0}, {"x1", &x1},
    {"x2", &x2}, {"x3", &x3},
    {"x4", &x4}, {"x5", &x5},
    {"x6", &x6}, {"x7", &x7},
    {"x8", &x8}, {"x9", &x9},

    {"x10", &x10}, {"x11", &x11},
    {"x12", &x12}, {"x13", &x13},
    {"x14", &x14},

    {"sy", &sy}, {"rv", &rv}
};

class label { // stores a program
    public:
        std::string name = "";
        std::vector<std::vector<std::string>> program;
        int argsTaken;
        int currentLine = 0;
        enum types returnType = nil; // by default
        std::vector<std::string> args;
        std::string calledFrom = "nil";
};

namespace bakeapple_libs {
    class library {
        public:
            std::string name;
            std::vector<label> allables;
            std::unordered_map<std::string, int> globals;
    };
}

std::vector<bakeapple_libs::library> loadedLibs = {};

namespace bakeapple_isa {
    std::unordered_map<std::string, std::function<int(std::string, std::string, label*, std::vector<label>*)>> instruction_set = {
        {"MOV", [](std::string dest, std::string val, label *c=nullptr, std::vector<label> *cc=nullptr){
            // MOV [dest] [val] : Replaces the value of dest with the value of val
            if (!regconv.count(dest)){
                throw bakeapple_err::invalidRegisterError();
            }
            reg *r = regconv[dest];
            if (regconv.count(val)){
                auto v = regconv[val]->strg;
                reg *r = regconv[dest];
                r->strg = std::stoi(val);
                r->type = integer;
                return 0;
            } else {
                r->strg = std::stoi(val);
            }
            sy.strg=1;
            return 0;
        }},
        {"ADD", [](std::string dest, std::string val, label *c=nullptr, std::vector<label> *cc=nullptr){
            // ADD [dest] [val] : Adds the value of val to the dest
            if (!regconv.count(dest)){
                throw bakeapple_err::invalidRegisterError();
            }
            reg *r = regconv[dest];
            if (regconv.count(val)){
                auto v = regconv[val]->strg;
                reg *r = regconv[dest];
                r->strg += std::stoi(val);
                r->type = integer;
                return 0;
            } else {
                r->strg += std::stoi(val);
            }
            sy.strg=1;
            return 0;
        }},
        {"SUB", [](std::string dest, std::string val, label *c=nullptr, std::vector<label> *cc=nullptr){
            // SUB [dest] [val] : Subtracts the value of val from the value of dest
            if (!regconv.count(dest)){
                throw bakeapple_err::invalidRegisterError();
            }
            reg *r = regconv[dest];
            if (regconv.count(val)){
                auto v = regconv[val]->strg;
                reg *r = regconv[dest];
                r->strg += std::stoi(val);
                r->type = integer;
                return 0;
            } else {
                r->strg += std::stoi(val);
            }
            sy.strg=1;
            return 0;
        }},
        {"MUL", [](std::string dest, std::string val, label *c=nullptr, std::vector<label> *cc=nullptr){
            // MUL [dest] [val] : Multiplies the value of dest by the value of val
            if (!regconv.count(dest)){
                throw bakeapple_err::invalidRegisterError();
            }
            reg *r = regconv[dest];
            if (regconv.count(val)){
                auto v = regconv[val]->strg;
                reg *r = regconv[dest];
                r->strg *= std::stoi(val);
                r->type = integer;
                return 0;
            } else {
                r->strg *= std::stoi(val);
            }
            sy.strg=1;
            return 0;
        }},
        {"DIV", [](std::string dest, std::string val, label *c=nullptr, std::vector<label> *cc=nullptr){
            // DIV [dest] [val] : Divides the value of dest by the value of val
            if (!regconv.count(dest)){
                throw bakeapple_err::invalidRegisterError();
            }
            reg *r = regconv[dest];
            if (regconv.count(val)){
                auto v = regconv[val]->strg;
                reg *r = regconv[dest];
                r->strg /= std::stoi(val);
                r->type = integer;
                return 0;
            } else {
                r->strg /= std::stoi(val);
            }
            sy.strg=1;
            return 0;
        }},
        {"WRT", [](std::string str, std::string rd, label *c=nullptr, std::vector<label> *cc=nullptr){
            // WRT [str] [rd]: Prints the value of str to the terminal. 'rd' cooresponds to one of the values below and different things will happen depending on its value  
            if (regconv.count(str)){
                switch (regconv[str]->type) {
                    case string:
                        str = regconv[str]->ldrstr();
                        break;

                    case !string:
                        str = std::to_string(regconv[str]->strg);
                        break;
                }
            } 
            switch (std::stoi(rd)){
            case 1:
                std::cout << str << "\n";
                std::cout.flush();
                break;
            
            case 2:
                std::cerr << str;
                std::cerr.flush();
                break;

            case 3:
                std::cerr << str << "\n";
                std::cerr.flush();
                break;

            default: // 0 typically
                std::cout << str;
                std::cout.flush();
                break;
            }
            sy.strg=1;
            return 0;
        }},
        {"NOP", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // NOP [redundant] [redundant] : No operation, does nothing
            return 0;
        }},
        {"RET", [](std::string val="", std::string type="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // RET [val] [type] : Returns to the label it was called from and pushes val into rv
            if (!typeconv.count(type)) {
                throw bakeapple_err::invalidTypeError();
            } else {
                rv.type=typeconv[type];
                switch (rv.type) {
                case integer:
                    rv.strg = std::stoi(val);
                    break;
                
                case string:
                    rv.strg = (uint64_t)(uintptr_t)val.c_str();
                    break;

                case nil:
                    if (val!="0"){
                        throw bakeapple_err::nilValueError();
                    } else {
                        rv.strg=0;
                    }
                    break;

                case boolean:
                    if (val!="true"||"false"){
                        throw bakeapple_err::booleanValueError();
                    } else {
                        if (val == "true"){
                            rv.strg=1;
                        } else {
                            rv.strg=0;
                        }
                    }
                    break;
                
                case character:
                    if (val.length()==1){
                        rv.strg=val[0];
                    } else {
                        throw bakeapple_err::characterValueError();
                    }
                    break;

                default:
                    break;
                }
            }
            for (label call : (*cc)) {
                if (call.name == (*c).calledFrom){
                    for (int i=call.currentLine; i<=call.program.size(); i++){
                        instruction_set[call.program[i][0]](call.program[i][1], call.program[i][2], &call, cc);
                    }
                }
            }
            rv.store(&val,rv.type);
            return 0;
        }},
        {"CALL", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // CALL [label name] [{array of parameters}] : Calls a label
            std::vector<std::string> r1;
            std::string a;
            bool f1 = false;

            r1=bakeapple_tkn::tokenize(val, ' ');
            for (auto& cal : (*cc)){
                if (cal.name == dest) {
                    cal.args=r1;
                    for (auto instr : cal.program) {
                        instruction_set[instr[0]](instr[1], instr[2], &cal, cc);
                    }
                }
            }
            sy.strg=1;
            return 0;
        }},
        {"STR", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // STR [dest] [{type, value}] : Stores a value in memory of the given type and stores a pointer to said value in the dest register, for integers works identically to MOV
            std::vector<std::string> r1;
            std::string a;  
            bool f1 = false;
            if (!regconv.count(dest)){
                throw std::exception();
            }
            reg *r = regconv[dest];
            
            r1=bakeapple_tkn::tokenize(val);

            auto *s = new std::string(r1[1]); // up to you (probably not you just the compiler which will do it automatically when a variable goes out of scope) to free ytour own damn memory!!!!!
            r->store(s, typeconv[r1[0]]);
            sy.strg=1;
            return 0;
        }},
        {"DEL", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // DEL [register] [redundant] : Deletes the value of the given variable
            if (!regconv.count(dest)){
                throw bakeapple_err::invalidRegisterError();
            }
            reg *r = regconv[dest];
            switch (r->type) {
                case !string:
                    r->type=nil;
                    r->strg=0;
                    break;

                case string:
                    std::string *rt = reinterpret_cast<std::string*>(r->strg);
                    delete(rt);
                    r->strg=0;
                    r->type=nil;
                    break;
            }
            sy.strg=1;
            return 0;
        }},
        {"CMP", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // CMP [value 1] [value 2] : Compares two values, storing the return value in sy, 1 or 0 depending on if they are equal
            if (regconv.count(dest)){
                reg *r = regconv[dest];
                if (r->type==string){
                    val=r->ldrstr();
                } else {
                    val=r->strg;
                }
            }
            if (regconv.count(val)){
                reg *r = regconv[val];
                if (r->type==string){
                    val=r->ldrstr();
                } else {
                    val=r->strg;
                }
            }
            if (val==dest){
                sy.strg=1;
            } else {
                sy.strg=0;
            }
            return 0;
        }},
        {"CMG", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // CMG [value 1] [value 2] : Compares two values, storing the return value in sy, 1 or 0 depending on if value 1 is greater than value 2, numeric operation
            if (std::stoi(dest)>std::stoi(val)){
                sy.strg=1;
            } else {
                sy.strg=0;
            }
            return 0;
        }},
        {"CGE", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // CGE [value 1] [value 2] : Compares two values, storing the return value in sy, 1 or 0 depending on if value 1 is greater than or equal to value 2, numeric operation
            if (std::stoi(dest)>=std::stoi(val)){
                sy.strg=1;
            } else {
                sy.strg=0;
            }
            return 0;
        }},
        {"CLE", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // CLE [value 1] [value 2] : Compares two values, storing the return value in sy, 1 or 0 depending on if value 1 is less than or equal to value 2, numeric operation
            if (std::stoi(dest)<=std::stoi(val)){
                sy.strg=1;
            } else {
                sy.strg=0;
            }
            return 0;
        }}, 
        {"CML", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // CML [value 1] [value 2] : Compares two values, storing the return value in sy, 1 or 0 depending on if value 1 is less than value 2, numeric operation
            if (std::stoi(dest)<std::stoi(val)){
                sy.strg=1;
            } else {
                sy.strg=0;
            }
            return 0;
        }},
        {"JE", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // JE [label name] [{ar, gu, ments}] : If sy is 1, call the given label
            if (sy.strg==1) { 
                instruction_set["CALL"](dest, val, c, cc);
                sy.strg=1;
            } else {
                sy.strg=0;
            }
            return 0;
        }},
        {"JNE", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // JNE [label name] [{ar, gu, ments}] : If sy is not 1, call the given label
            if (sy.strg!=1) { 
                instruction_set["CALL"](dest, val, c, cc);
                sy.strg=1;
            } else {
                sy.strg=0;
            }
            return 0;
        }},
        {"REQ", [](std::string fn = "", std::string lnm = "", label *c = nullptr, std::vector<label> *cc = nullptr) {
            // REQ [file name] [library reference] : Loads a library file into memory
            std::unordered_map<std::string, int> globls;
            std::vector<label> allables;
            std::vector<label> slabels;
            std::ifstream file(fn);

            int cPtr = -1;
            for (std::string ln; std::getline(file, ln); ) {
                if (ln.substr(0, 6) == "GLOBAL" && ln.size() > 8) {
                    globls.insert({ln.substr(8), 0});
                }

                if (ln[0] == '.' && ln.find(':') != std::string::npos) {
                    label e;
                    e.name = ln.substr(1, ln.find(':') - 1);
                    cPtr++;
                    slabels.push_back(e);
                    if (globls.find(e.name) != globls.end()) {
                        globls[e.name] = cPtr;
                    }
                }

                if (!slabels.empty()) {
                    if (ln == "END") {
                        allables.push_back(slabels[0]);
                        slabels.pop_back();
                    } else if (ln[0] != '.') {
                        slabels[0].program.push_back(bakeapple_tkn::tokenize(ln));
                    }
                }
            }

            bakeapple_libs::library l;
            l.globals = globls;
            l.allables = allables;
            l.name = lnm;
            loadedLibs.push_back(l);
            sy.strg=1;
            return 0;
        }},
        {"LIBC", [](std::string dest="", std::string val="", label *ccc=nullptr, std::vector<label> *cc=nullptr){
            // LIBC [{label name, source library name}] [{array, of, args}] : Explicitly calls a label from its name and library
            std::vector<std::string> r1=bakeapple_tkn::tokenize(dest, ' ');
            std::vector<std::string> r2=bakeapple_tkn::tokenize(val, ' ');
            for (auto& lib : loadedLibs) {
                if (r1[1]!=lib.name) {
                    continue;
                } else {
                    if (lib.globals.count(r1[0])){ 
                        label c = lib.allables[lib.globals[r1[0]]];
                        c.args=r2;
                        for (auto instr : c.program) {
                            instruction_set[instr[0]](instr[1], instr[2], &c, cc);
                        }
                    }
                }
            }
            sy.strg=1;
            return 0;
        }},
        {"INP", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // INP [register] [redundant] : Get user input and store it as a string in the given register
            std::string ss;
            std::cin >> ss;
            if (regconv.count(dest)){
                reg *r = regconv[dest];
                r->store(&ss, string);
            } else {
                throw bakeapple_err::invalidRegisterError();
            }
            sy.strg=1;
            return 0;
        }},
        {"RAC", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // RAC [new register name] [integer value] : Allocates memory for a new register
            reg *r = new reg;
            r->type=integer;
            r->strg=std::stoi(val);
            regconv.insert({dest, r});
            return 0;
        }},
        {"DRG", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // DRG [register name] [redundant] : Deallocates a register from memory
            if (!regconv.count(dest)){
                throw bakeapple_err::invalidRegisterError();
            }
            reg *r = regconv[dest];
            delete(r);
            regconv.erase(dest);
            return 0;
        }},
        {"ACP", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // ACP [register] [parameter number] : Makes a pointer to the given label parameter in the given register
            if (!regconv.count(dest)){
                throw bakeapple_err::invalidRegisterError();
            }
            reg *r = regconv[dest];
            int v = std::stoi(val);
            std::string p = c->args[v];
            r->store(&p, string);
            
            sy.strg=1;
            return 0;
        }},
        {"FOR", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // FOR [{start, condition, increment}] [label name] : Repeats a label while the condition is true
            std::vector<std::string> body=bakeapple_tkn::tokenize(dest, ' ');
            std::vector<std::string> init=bakeapple_tkn::tokenize(body[0], ' ');
            std::vector<std::string> cond=bakeapple_tkn::tokenize(body[1], ' ');
            std::vector<std::string> incr=bakeapple_tkn::tokenize(body[2], ' ');

            instruction_set[init[0]](init[1], init[2], c, cc);
            instruction_set[cond[0]](cond[1], cond[2], c, cc);
            
            std::cout<<std::to_string(sy.strg)<<"\n";

            while (sy.strg==1){ // sy is a special register indicating whether or not the last operation was successful, it will run the conditional instruction and if it is unsuccessful, it will break execution
                instruction_set["CALL"](val, "{}", c, cc);
                instruction_set[incr[0]](incr[1], incr[2], c, cc);
                instruction_set[cond[0]](cond[1], cond[2], c, cc);
            }
            sy.strg=1;
            return 0;
        }},
        {"SRT", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // SRT [type] [redundant] : Sets the return type of the current label
            c->returnType=typeconv[dest];
            sy.strg=1;
            return 0;
        }},
        {"CHT", [](std::string dest="", std::string val="", label *c=nullptr, std::vector<label> *cc=nullptr){
            // CHT [type] [register] : Sets the type of the given register to the given type, use with caution
            enum types type = typeconv[dest];
            if (regconv.count(val)){
                reg *r = regconv[val];
                r->type=type;
            } else {
                throw bakeapple_err::invalidRegisterError();
            }
            return 0;
        }},
    };
}

namespace bakeapple_emb {

    int runScript(std::vector<std::string> args, std::vector<std::vector<std::string>> script){
        label c;
        c.name="main";
        std::vector<label> cc = { c };

        c.program=script;
        c.args=args;

        for (auto x : c.program){
            for (const auto& instr : c.program) {
                bakeapple_isa::instruction_set[instr[0]](instr[1], instr[2], &c, &cc);
            }
        } return 0; 
    }

/*  
    

    bakeapple_emb::runScript(
        {"example", "of", "args"},
        {
            {"MOV", "x0", "1"},
            {"ADD", "x1", "x0"}
        }
    );


                                        */ 
}
