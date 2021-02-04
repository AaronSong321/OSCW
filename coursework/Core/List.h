
#include "Collection.h"
#include "Comparator.h"
#include <iostream>
using namespace std;
#include "IntRange.h"

namespace Commons::Collections {
    template <class T>
    class List;

    template <class T>
    class ListNode {
    private:
        T _data;
        SharedPointer<ListNode<T>> _next;
        WeakPointer<ListNode<T>> _prev;
        friend class List<T>;

    public:
        T Data() const { return _data; }
        SharedPointer<ListNode<T>> Next() const { return _next; }
        ListNode(T data): _data(data), _next(nullptr), _prev(nullptr) {
        }
    };

    namespace __impl{
        template <class T>
        class ListIterator: public IEnumerator<T> {
            SharedPointer<ListNode<T>> _cur;
            const SharedPointer<ListNode<T>> _root;
            bool _startState;
        public:
            ListIterator(const List<T>* list): _cur(list->Root()), _root(list->Root()), _startState(true) {
            }
            bool MoveNext() override {
                if (!_cur)
                    return false;
                if (_startState) {
                    _startState = false;
                    return true;
                }
                _cur = _cur->Next();
                return _cur != _root;
            }
            SharedPointer<T> Get() const override {
                return MakeShared<T>(_cur->Data());
            }
        };
    }


    template <class T>
    class List: public ICollection<T> {
    private:
        SharedPointer<ListNode<T>> _root;
        int _count;

    public:
        List(): _root(nullptr), _count(0) {
        }

    private:
        void AddAfter(SharedPointer<ListNode<T>> newNode, SharedPointer<ListNode<T>> oldNode) {
            newNode->_next = oldNode->_next;
            newNode->_prev = oldNode;
            oldNode->_next->_prev = newNode;
            oldNode->_next = newNode;
            ++_count;
        }

        void RemoveNode(SharedPointer<ListNode<T>> node) {
            if (!node->_prev.Pin() || !node->_next) {
                node = node;
            }
            node->_next->_prev = node->_prev.Pin();
            node->_prev.Pin()->_next = node->_next;
            if (node == _root) {
                _root = _root->_next;
                if (_count == 1) {
                    _root = nullptr;
                }
            }
            --_count;
        }

    public:
        SharedPointer<ListNode<T>> Root() const { return _root; }

        SharedPointer<ListNode<T>> Insert(T elem, SharedPointer<ListNode<T>> node){
            if (!node)
                return SharedPointer<ListNode<T>>();
            auto newNode = MakeShared<ListNode<T>>(elem);
            AddAfter(newNode, node);
            return newNode;
        }
        SharedPointer<ListNode<T>> AddToHead(T elem) {
            auto newNode = MakeShared<ListNode<T>>(elem);
            if (_root) {
                AddAfter(newNode, _root->_prev.Pin());
            } else {
                newNode->_next = newNode;
                newNode->_prev = newNode;
                ++_count;
            }
            _root = newNode;
            return newNode;
        }
        SharedPointer<ListNode<T>> AddToTail(T elem) {
            auto newNode = MakeShared<ListNode<T>>(elem);
            if (_root) {
                AddAfter(newNode, _root->_prev.Pin());
            } else {
                newNode->_next = newNode;
                newNode->_prev = newNode;
                _root = newNode;
                ++_count;
            }
            return newNode;
        }

        void Add(T elem) override {
            AddToTail(elem);
        }

        SharedPointer<ListNode<T>> Find(T elem, SharedPointer<IValueEqualityComparator<T>> equalityComparator) const {
            if (!equalityComparator || !_root)
                return nullptr;
            const auto end = _root->_next;
            auto node = _root;
            do {
                if (equalityComparator->Invoke(node->Data(), elem)) {
                    return node;
                }
                node = node->_next;
            } while (node != end);
            return nullptr;
        }

        bool Contains(T elem) const override {
            const auto compare = GetDefaultValueEqualityComparator<T>();
            return Find(elem, compare);
        }

        void Remove(T elem, SharedPointer<IValueEqualityComparator<T>> equalityComparator) {
            if (!equalityComparator || !_root)
                return;
            const auto end = _root;
            auto node = _root;
            SharedPointer<ListNode<T>> deleteList[_count];
            int deleteIndex = 0;
            do {
                if (equalityComparator->Invoke(node->Data(), elem)) {
                    deleteList[deleteIndex++] = node;
                }
                node = node->_next;
            } while (node != end);
            int p = 0;
            while (p < deleteIndex) {
                RemoveNode(deleteList[p++]);
            }
        }

        void Remove(T elem) override {
            Remove(elem, GetDefaultValueEqualityComparator<T>());
        }

        ~List() override {
            if (_root)
                _root->_next = nullptr;
        }

        SharedPointer<IEnumerator<T>> GetEnumerator() const override {
            auto enumerator = MakeShared<__impl::ListIterator<T>>(this);
            return enumerator.template StaticCast<IEnumerator<T>>();
        }

        int GetCount() const override {
            return _count;
        }

        void Clear() override {
            if (_root) 
                _root->_next = nullptr;
            _root = nullptr;
            _count = 0;
        }
        
        SharedPointer<ListNode<T>> GetHead() const {
            return _root;
        }
        SharedPointer<ListNode<T>> GetTail() const {
            return _root ? _root->_prev.Pin() : nullptr;
        }
        T* Get(int index) const {
            if (index>0 && index<_count) {
                auto node = _root;
                auto f = &node;
                MacroDeclareLambdaFunctor1(look, [f], SharedPointer<int>, void, { *f = f->_next; });
                To(0, index).ForEach(look);
                return node.Get();
            }
            if (index<0 && -index>=-_count){
                auto node = _root;
                auto f = &node;
                MacroDeclareLambdaFunctor1(look, [f], SharedPointer<int>, void, { *f = f->_prev; });
                To(0, -index).ForEach(look);
                return node.Get();
            }
            ShallThrow(std::out_of_range("List index out of range"));
            return nullptr;
        }
        T& Set(int index, const T& item) {
            if (index>0 && index<_count) {
                auto node = _root;
                auto f = &node;
                MacroDeclareLambdaFunctor1(look, [f], SharedPointer<int>, void, { *f = f->_next; });
                To(0, index).ForEach(look);
                auto prev = node->_prev;
                RemoveNode(node);
                if (prev == node->_prev) {
                    AddToTail(item);
                } else {
                    AddAfter(prev, MakeShared<ListNode<T>>(item));
                }
                goto ret;
            }
            if (index<0 && -index>=-_count) {
                auto node = _root;
                auto f = &node;
                MacroDeclareLambdaFunctor1(look, [f], SharedPointer<int>, void, { *f = f->_prev; });
                To(0, index).ForEach(look);
                auto prev = node->_prev;
                RemoveNode(node);
                if (prev == node->_prev) {
                    AddToTail(item);
                } else {
                    AddAfter(prev, MakeShared<ListNode<T>>(item));
                }
                goto ret;
            }
            ShallThrow(std::out_of_range("List index out of range"));
            ret: return item;
        }

//        void Print() const {
//            cout<<"Traverse list count="<<_count<<endl;
//            if (!_count)
//                return;
//            auto node = _root;
//            do {
//                cout << node << ", next="<<node->_next.Get()<<" "<<node->_next<<", prev="<<node->_prev.Pin().Get()<<" "<<node->_prev<<endl;
//                node = node->_next;
//            } while (node != _root);
//        }
    };
}