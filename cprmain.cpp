#include <iostream>
#include <cpr/cpr.h>
#include <vector>
#include <string>
#include <fstream>

std::string extractMessage(std::string res){
    //std::cout << res << std::endl;
    std::string firststr = "text\":\"\\n\\n";
    std::string secondstr = "\",\"index";
    auto start = res.find(firststr);
    if(start == std::string::npos){
        std::cout << "Not found." << std::endl;
        return "";
    }
    //std::cout << "START: " << start << std::endl;
    auto end = res.find(secondstr);
    //std::cout << "  END: " << end << std::endl;
    
    std::string message = res.substr(start+11, end-(start+11));
    std::string fixed = "";

    for(int i=0; i<message.size(); i++){
        if(message[i]=='\\'){
            if(message[i+1]=='n'){
                fixed+='\n';
                i++;
            }
        }else{
            fixed+=message[i];
        }
    }

    return fixed;
}

int main(int argc, char *argv[]) {
    std::string url = "https://api.openai.com/v1/completions";
    std::string api_key = "";
    std::string prompt = "";
    std::string model = "text-davinci-003";
    float temperature = 0.5f;

    std::ifstream keyFile("api_key.txt");
    if(keyFile.is_open()){
        getline(keyFile, api_key);
        //std::cout << api_key << std::endl;
        keyFile.close();
    }else{
        std::cout << "Failed to find api key file. Should be in api_key.txt" << std::endl;
        return 1;
    }

    if(argc<2){
        std::cout << "Must be called with command-line args." << std::endl;
        return 1;
    }else{
        for(int i=1; i<argc; i++){
            prompt += std::string(argv[i])+" ";
        }
        //std::cout << "Prompt: " << prompt << std::endl;
    }

    std::string body_string = "{ \"model\": \""+model+"\", \"prompt\": \"" + prompt + "\", \"temperature\": 0.5, \"max_tokens\": 1000 }";

    cpr::Response r = cpr::Post(cpr::Url{url},
                                cpr::Header{{"Content-Type", "application/json"},
                                            {"Authorization", "Bearer " + api_key}},
                                cpr::Body{body_string},
                                cpr::VerifySsl{false}); // Only use this option in development mode, not recommended in production!
    
 

    // parse the JSON response
    std::stringstream ss(r.text);
    std::vector<std::string> tokens;
    std::string token;
    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }
    std::string message = "";
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i].find("\"text\"") != std::string::npos) {
            message = tokens[i+1].substr(0, tokens[i + 1].size() - 2);
            break;
        }
    }
    //std::cout << tokens[4].substr(24, tokens[4].size()-1) << std::endl;
    //std::cout << std::endl << tokens[5] << std::endl;

    //std::map<std::string, std::string> map = parse_json(r.text);
    //std::cout << map.find("text")->second << std::endl;
    
    //std::cout << std::endl << std::endl << r.text << std::endl;

    //std::cout << std::endl << std::endl;

    std::cout << extractMessage(r.text) << std::endl;

    return 0;
}