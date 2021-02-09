//
// Created by Dimanche on 09/02/2021.
//

#ifndef CORE_LINEARCOLLECTION_H
#define CORE_LINEARCOLLECTION_H
namespace Commons::Collections {
    template <class T>
    class Queue {
    public:
        virtual void Enqueue(T elem) = 0;
        virtual T Dequeue() = 0;
        virtual T Front() const = 0;
    };
}

#endif //CORE_LINEARCOLLECTION_H
