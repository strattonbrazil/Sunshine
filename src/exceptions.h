#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

class KeyErrorException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "queried key does not exist";
    }
};


#endif // EXCEPTIONS_H
