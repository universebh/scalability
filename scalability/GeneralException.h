//
// Created by 陆清 on 3/24/20.
//

#ifndef H4D1_GENERALEXCEPTION_H
#define H4D1_GENERALEXCEPTION_H
#include <exception>
#include <string>
class GeneralException : public std::exception {
private:

    std::string message;
public:
    explicit GeneralException(std::string message): message(message){}

    virtual const char *what(){
        return message.c_str();
    }
};

#endif //H4D1_GENERALEXCEPTION_H
