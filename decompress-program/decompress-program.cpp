#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>


void get(int index, int & counter, std::string* dict, std::vector<int> &codeArr, std::string & result, std::ofstream & o_file) {
    int code = codeArr[index];
    
    // if code exists in the dictionary add preciding code text + first char of the code text to the dictionary
    if (dict[code] != "") {
        result += dict[code];
        o_file << dict[code];
        std::string tq = dict[codeArr[index - 1]];
        if (counter <4095){
        dict[counter + 1] = tq + dict[code][0];
        counter++;
        }
    }
    //if code does not exist in the dictionary add preciding code text + first char of the 
    //preciding code text to the dictionary and the output
    else {
        std::string t = dict[codeArr[index - 1]];
        std::string fc = std::string(1, t[0]);
        dict[code] = t + fc;
        result += dict[code];
        o_file << dict[code];
        counter++;
    }
}


int main(){

    // Create string dictionary and add the first 256 chars.
    std::string * dictArr = new std::string[4096];
    for (int i = 0; i < 256; i++) {
        char temp = i;
        dictArr[i] = std::string(1, temp);
    }

    std::ifstream in_file("compout");
    std::ofstream o_file("decompout");

    std::string line;
    int value;
    //set the current char dictionary key to 255
    int counter = 255;
    // read all compressed codes to a vector.
    std::vector<int> codeArr;
    if (std::getline(in_file, line)){
        std::istringstream iss(line);
        while(iss >> value){
            codeArr.push_back(value);
        }
    }
   

    std::string result = "";
    result = dictArr[codeArr[0]];
    o_file << dictArr[codeArr[0]];

    // Iterate through compressed codes
    for (int i = 1; i< codeArr.size(); i++) {
        get(i, counter, dictArr, codeArr, result, o_file);

    }
    in_file.close();
    o_file.close();
    std::cout << result << std::endl;
}
