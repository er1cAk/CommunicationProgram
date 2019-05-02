//
// Created by Erik Dvorcak on 2019-04-23.
//

#ifndef COMMUNICATIONSERVER_AURORAEXCEPTION_H
#define COMMUNICATIONSERVER_AURORAEXCEPTION_H

#include <exception>
using namespace std;

class AuroraConnectException : public exception {
public:
    virtual const char* what() const throw()
    {
        return "Having Aurora connection problem.";
    }
};

class AuroraSendingRequestException : public exception {
public:
    virtual const char* what() const throw()
    {
        return "Aurora sending request problem.";
    }
};

class AuroraReceivingResponseException : public exception {
public:
    virtual const char* what() const throw()
    {
        return "Aurora receiving response problem.";
    }
};

#endif //COMMUNICATIONSERVER_AURORAEXCEPTION_H
