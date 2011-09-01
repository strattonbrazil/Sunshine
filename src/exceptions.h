#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <QString>

class KeyErrorException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "queried key does not exist";
    }
};

QString parse_python_exception();

#endif // EXCEPTIONS_H
