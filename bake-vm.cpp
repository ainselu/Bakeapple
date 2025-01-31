#include "bakeapple.h"
#include <CLI/CLI.hpp>

std::string globl;
std::string version = "0.1.0";

std::vector<label> loadFromFile(std::string fn){
    std::vector<std::vector<std::string>> program;
    std::vector<label> labels;
    std::vector<label> slabels;
    std::ifstream file(fn);
    int lnPtr = 0;
    bool flag = false;
    int strtln, en;

    for (std::string ln; std::getline(file, ln); lnPtr++ ) {
        if (ln.substr(0,6) == "GLOBAL" && lnPtr == 0){
            globl = ln.substr(8);
        }
        if (ln[0] == '.'){
            label e;
            e.name = ln.substr(1, ln.find(':')-1);
            slabels.push_back(e);
        }

        if (!slabels.empty()){
            if (ln=="END") {
                labels.push_back(slabels[0]);
                slabels.pop_back();
            } else if (ln[0]!='.') {
                slabels[0].program.push_back(bakeapple_tkn::tokenize(ln));
            }
        }
    }
    return labels;
}


int run(std::vector<label> labels, bool debug = false){
    for (auto c : labels) {
        if (globl == c.name) {
            for (const auto& instr : c.program) {
                if (debug==true){
                    std::cout << "\033[97m[\033[95mDEBUG\033[97m] | Executing instruction parsed as '" << instr[0] << "', '" << instr[1] << "', '" << instr[2] << "'\033[0m\n";
                    try {
                        bakeapple_isa::instruction_set[instr[0]](instr[1], instr[2], &c, &labels);
                    } catch (const std::exception& e) {
                        std::cout << "\033[97m[\033[91mERROR\033[97m] | Exception caught: " << e.what() << "\033[0m\n";
                    }
                } else {
                    bakeapple_isa::instruction_set[instr[0]](instr[1], instr[2], &c, &labels);
                }
                
                c.currentLine++;
            }
        }
    }
    if (debug){
        std::cout << "\033[97m[\033[95mDEBUG\033[97m] | Program finished execution\033[0m\n";
        std::cout << "\033[97m[\033[95mDEBUG\033[97m] | Register values:\n";
        std::cout << "x0: " << std::to_string(x0.strg) << ", x1: " << std::to_string(x1.strg) << "\n";
        std::cout << "x2: " << std::to_string(x2.strg) << ", x3: " << std::to_string(x3.strg) << "\n";
        std::cout << "x4: " << std::to_string(x4.strg) << ", x5: " << std::to_string(x5.strg) << "\n";
        std::cout << "x6: " << std::to_string(x6.strg) << ", x7: " << std::to_string(x7.strg) << "\n";
        std::cout << "x8: " << std::to_string(x8.strg) << ", x9: " << std::to_string(x9.strg) << "\n";
        std::cout << "x10: " << std::to_string(x10.strg) << ", x11: " << std::to_string(x11.strg) << "\n";
        std::cout << "x12: " << std::to_string(x12.strg) << ", x13: " << std::to_string(x13.strg) << "\n";
        std::cout << "x14: " << std::to_string(x14.strg) << "\n";

        std::cout << "sy: " << std::to_string(sy.strg) << ", rv: " << std::to_string(rv.strg) << "\n\033[0m";
    }
    globl = "";
    return 0;
}

int main(int argc, char** argv){ 
    CLI::App vm("bake-vm");
    std::vector<std::string> parameters;
    vm.add_option("--rf, --readfile", parameters, "Executes a given file")
        ->expected(1);

    vm.add_option("--d, --debug", parameters, "Runs a file in debug mode")
        ->expected(1);

    vm.add_option("--v, --version", "Prints the current version of bake-vm")
        ->expected(0);

    CLI11_PARSE(vm, argc, argv);

    if (vm.count("--rf")){
        std::string fn = vm["--rf"]->as<std::string>();
    } else if (vm.count("--d")){
        std::string fn = vm["--d"]->as<std::string>();
        run(loadFromFile(fn), true);
    } 
}               