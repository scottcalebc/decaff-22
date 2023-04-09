#pragma once


class Acceptor;

/**
 * @brief Visitor interface that will visit nodes that have an acceptor function
 * 
 */
class Visitor {

    protected:
        Visitor() {};

    public:
        virtual void visit(Acceptor *a) = 0;
};



/**
 * @brief Acceptor interface that will accept someone from the Visitor Interface
 * 
 */
class Acceptor {
    protected:
        Acceptor() {};

    public:
        virtual void accept(Visitor *v) = 0;
};
