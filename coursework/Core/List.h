
#include "Collection.h"

namespace Commons::Collections {
    template <class T>
    class List;

    template <class T>
    class ListNode {
    private:
        SharedPointer<T> _data;
        SharedPointer<ListNode<T>> _next;
        WeakPointer<ListNode<T>> _prev;
        friend class List<T>;

    public:
        SharedPointer<T> Data() const { return _data; }
        SharedPointer<ListNode<T>> Next() const { return _next; }
        ListNode(T data): _data(MakeShared<T>(data)), _next(nullptr), _prev(nullptr) {
        }
    };

    namespace __impl{
        template <class T>
        class ListIterator: public IEnumerator<T> {
            SharedPointer<ListNode<T>> _cur;
            const SharedPointer<ListNode<T>> _root;
            bool _startState;
        public:
            ListIterator(const List<T>* list): _root(list->Root()), _cur(list->Root()), _startState(true) {
            }
            virtual bool MoveNext() override {
                if (!_cur)
                    return false;
                if (_startState) {
                    _startState = false;
                    return true;
                }
                _cur = _cur->Next();
                return _cur != _root;
            }
            virtual SharedPointer<T> Get() const override {
                return _cur->Data();
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

        void Remove(SharedPointer<ListNode<T>> node) {
            node->_next->_prev = node->_prev.Pin();
            node->_prev.Pin()->_prev = node->_next;
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
            }
            _root = newNode;
            return newNode;
        }
        SharedPointer<ListNode<T>> AddToTail(T elem) {
            auto newNode = MakeShared<ListNode<T>>(elem);
            if (_root) {
                AddAfter(newNode, _root);
            } else {
                newNode->_next = newNode;
                newNode->_prev = newNode;
            }
            _root = newNode;
            return newNode;
        }

        virtual void Add(T elem) override {
            AddToTail(elem);
        }

        SharedPointer<ListNode<T>> Find(T elem, SharedPointer<IEqualityComparator<T>> equalityComparator) const {
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

        virtual bool Contains(T elem) const override {
            const auto compare = GetDefaultEqualityComparator<T>();
            return Find(elem, compare);
        }

        void Remove(T elem, SharedPointer<IEqualityComparator<T>> equalityComparator) {
            if (!equalityComparator || !_root)
                return;
            const auto end = _root->_next;
            auto node = _root;
            SharedPointer<ListNode<T>> deleteList[_count];
            int deleteIndex = 0;
            do {
                if (equalityComparator->Invoke(node->Data(), elem)) {
                    deleteList[deleteIndex++] = node;
                }
                node = node->_next;
            } while (node != end);
            deleteIndex = 0;
            while (deleteList[deleteIndex++]) {
                Remove(node);
            }
        }

        virtual void Remove(T elem) override {
            Remove(elem, GetDefaultEqualityComparator<T>());
        }

        virtual ~List() override {
            if (_root)
                _root->_next = nullptr;
        }

        virtual SharedPointer<IEnumerator<T>> GetEnumerator() const override {
            auto enumerator = MakeShared<__impl::ListIterator<T>>(this);
            return enumerator.template StaticCast<IEnumerator<T>>();
        }

        virtual int GetCount() const override {
            return _count;
        }

        virtual void Clear() override {
            if (_root) 
                _root->_next = nullptr;
            _root = nullptr;
            _count = 0;
        }
    };
}