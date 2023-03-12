#include <iostream>
#include <cpr/cpr.h>
#include <vector>
#include <string>
#include <fstream>

/*Extracts the actual ChatGPT message from the response json*/
std::string extractMessage(std::string res){

    //String immediately before the message
    std::string firststr = "text\":\"\\n\\n";

    //String immediately after the message
    std::string secondstr = "\",\"index";

    auto start = res.find(firststr);
    if(start == std::string::npos){
        std::cout << "Not found." << std::endl;
        return "";
    }
    auto end = res.find(secondstr);
    
    std::string message = res.substr(start+11, end-(start+11));
    std::string fixed = "";

    //Convert \n to single character '\n' to preserve newline
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

/*Reads in and returns users ChatGPT api key saved to file api_key.txt*/
std::string readApiKey(){
    std::string key="";
    std::ifstream keyFile("api_key.txt");
    if(keyFile.is_open()){
        getline(keyFile, key);
        keyFile.close();
    }else{
        std::cout << "Failed to find api key file. Should be in api_key.txt" << std::endl;
        return nullptr;
    }
    return key;
}

int main(int argc, char *argv[]) {
    std::string url = "https://api.openai.com/v1/completions";
    std::string api_key = "";
    std::string prompt = "";
    std::string model = "text-davinci-003";

    //Read in user's ChatGPT api key
    api_key = readApiKey();

    //Set command-line arguments as the ChatGPT prompt
    if(argc<2){
        std::cout << "Must be called with command-line args." << std::endl;
        return 1;
    }else{
        for(int i=1; i<argc; i++){
            prompt += std::string(argv[i])+" ";
        }
    }

    std::string body_string = "{ \"model\": \""+model+"\", \"prompt\": \"" + prompt + "\", \"temperature\": 0.5, \"max_tokens\": 1000 }";

    cpr::Response r = cpr::Post(cpr::Url{url},
                                cpr::Header{{"Content-Type", "application/json"},
                                            {"Authorization", "Bearer " + api_key}},
                                cpr::Body{body_string},
                                cpr::VerifySsl{false}); // Only use this option in development mode, not recommended in production!
    

    std::cout << extractMessage(r.text) << std::endl;

    return 0;
}