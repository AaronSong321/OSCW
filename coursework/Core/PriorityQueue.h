//#pragma once
//
#include "Memory.h"
#include "Exception.h"
#include "Collection.h"
#include "IntRange.h"
#include "Comparator.h"
#include "Forward.h"

//
//#include <iostream>
//using namespace std;
//
//namespace Commons
//{
//    template<class TKey>
//    struct FibonacciNode;
//    template<class TKey>
//    class FibonacciHeap;
//    namespace __impl_PriorityQueue
//    {
//        template<class TKey>
//        struct FibonacciNodeChildrenEnumerator;
//    }
//
//    template<class TKey>
//    struct FibonacciNode: public IComparable<FibonacciNode<TKey>>, public ManagedObject<FibonacciNode<TKey>>
//    {
//    private:
//        TKey _key;
//        WeakPointer<FibonacciNode<TKey>> _left;
//        SharedPointer<FibonacciNode<TKey>> _right;
//        SharedPointer<FibonacciNode<TKey>> _child;
//        WeakPointer<FibonacciNode<TKey>> _parent;
//        int _degree;
//        bool _mark;
//        bool _minimumCausedByDeletion;
//        WeakPointer<FibonacciHeap<TKey>> _tree;
//
//        friend class FibonacciHeap<TKey>;
//        friend class __impl_PriorityQueue::FibonacciNodeChildrenEnumerator<TKey>;
//
//        void AddChild(SharedPointer<FibonacciNode<TKey>> node) {
//            //ThrowIf0(node);
//            if (!CompareNode(this, node.GetRawPointer()))
//                ShallThrow("Cannot add this as a child");
//            if (!_child){
//                _child=node;
//                node->_left=node;
//                node->_right=node;
//            }
//            else if (_child->_right==_child){
//                _child->_right=node;
//                _child->_left=node;
//                node->_right=_child;
//                node->_left=_child;
//            }
//            else{
//                _child->_right->_left=node;
//                node->_right=_child->_right;
//                _child->_right=node;
//                node->_left=_child;
//            }
//            node->_parent = ManagedObject<FibonacciNode<TKey>>::SharedPointerToThis();
//            ++_degree;
//        }
//
//        void DeleteChild(SharedPointer<FibonacciNode<TKey>> node){
//            ThrowIf0(node);
//            if (!CompareNode(this, node.GetRawPointer()))
//                ShallThrow("Cannot add this as a child");
//            if (_child->_left == _child && _child == node){
//                _child=0;
//            }
//            else{
//                node->_left.Pin()->_right=node->_right;
//                node->_right->_left=node->_left;
//                if (_child==node)
//                    _child=node->_right;
//            }
//            node->_parent=0;
//            node->_left=0;
//            node->_right=0;
//            --_degree;
//        }
//
//        SharedPointer<IEnumerable<FibonacciNode<TKey>>> GetChildren(){
//            auto src = EnumeratorToEnumerable(MakeShared<__impl_PriorityQueue::FibonacciNodeChildrenEnumerator<TKey>>(ManagedObject<FibonacciNode<TKey>>::SharedPointerToThis()).template StaticCast<IEnumerator<FibonacciNode<TKey>>>());
//            return src;
//        }
//
//    public:
//        static int CompareNode(SharedPointer<FibonacciNode<TKey>> lhs, SharedPointer<FibonacciNode<TKey>> rhs) {
//            return CompareNode(lhs.GetRawPointer(), rhs.GetRawPointer());
//        }
//        static int CompareNode(FibonacciNode<TKey>* lhs, FibonacciNode<TKey>* rhs){
//            ThrowIf0(lhs);
//            ThrowIf0(rhs);
//            if (lhs->_tree.Pin() != rhs->_tree.Pin())
//            ShallThrow(0);
//            if (lhs->_minimumCausedByDeletion) {
//                if (rhs->_minimumCausedByDeletion)
//                    return 0;
//                return -1;
//            }
//            if (rhs->_minimumCausedByDeletion)
//                return 1;
//            return lhs->_tree.Pin()->CompareKey(lhs->Key(), rhs->Key());
//        }
//
//        using IComparable<FibonacciNode<TKey>>::CompareTo;
//        int CompareTo(FibonacciNode<TKey> other) const override {
//            if (_tree.Pin() != other._tree.Pin())
//                ShallThrow(0);
//            if (_minimumCausedByDeletion){
//                if (other._minimumCausedByDeletion)
//                    return 0;
//                return -1;
//            }
//            if (other._minimumCausedByDeletion)
//                return 1;
//            return _tree.Pin()->CompareKey(_key, other._key);
//        }
//
//        TKey Key() const {
//            return _key;
//        }
//        SharedPointer<FibonacciNode<TKey>> GetChild() const { return _child; }
//        SharedPointer<FibonacciNode<TKey>> GetLeft() const { return _left.Pin(); }
//        SharedPointer<FibonacciNode<TKey>> GetRight() const { return _right; }
//
//        FibonacciNode(TKey key, SharedPointer<FibonacciHeap<TKey>> tree):_key(key),_left(nullptr),_right(nullptr),_child(nullptr),
//                _parent(nullptr),_degree(0),_mark(false),_minimumCausedByDeletion(false),_tree(tree) {
//        }
//
//    private:
//        void DestroyRing() {
//            if (_child)
//                if (_child->_right) {
//                    _child->_right->DestroyRing();
//                    _child->_right = nullptr;
//                }
//        }
//
//    public:
//        virtual ~FibonacciNode() {
//        }
//    };
//
//    namespace __impl_PriorityQueue
//    {
//        template<class TKey>
//        class FibonacciNodeChildrenEnumerator : public Collections::IEnumerator<FibonacciNode<TKey>>
//        {
//        private:
//            SharedPointer<FibonacciNode<TKey>> _iter;
//            int _index;
//            const int _degree;
//
//        public:
//            FibonacciNodeChildrenEnumerator(SharedPointer<FibonacciNode<TKey>> root) : _iter(root->_child->_left.Pin()), _index(0), _degree(root->_degree) {
//
//            }
//            bool MoveNext() noexcept override {
//                if (_index++ == _degree)
//                    return false;
//                _iter = _iter->_right;
//                return true;
//            }
//            SharedPointer<FibonacciNode<TKey>> Current() const noexcept override {
//                return _iter;
//            }
//        };
//    }
//
//    template<class TKey>
//    class FibonacciHeap: public Collections::IInputCollection<TKey>, public ManagedObject<FibonacciHeap<TKey>>
//    {
//    private:
//        SharedPointer<FibonacciNode<TKey>> _min;
//        int _degree;
//        const SharedPointer<IComparator<TKey>> _comparator;
//
//        void AddToRootList(SharedPointer<FibonacciNode<TKey>> node){
//            ThrowIf0(node);
//            if (!_min){
//                _min=node;
//                node->_left=node->_right=node;
//            } else {
//                _min->_left.Pin()->_right = node;
//                node->_left=_min->_left;
//                _min->_left=node;
//                node->_right=_min;
//            }
//            node->_parent = nullptr;
//        }
//
//        void RemoveFromRootList(SharedPointer<FibonacciNode<TKey>> node){
//            ThrowIf0(node);
//            if (_min->_left.Pin() == _min){
//                if (node != _min)
//                    ShallThrow("Incorrect node removal");
//                _min = 0;
//            } else {
//                node->_left.Pin()->_right=node->_right;
//                node->_right->_left=node->_left;
//            }
//            node->_parent = nullptr;
//        }
//
//        void Consolidate(){
//            SharedPointer<FibonacciNode<TKey>> a[_degree+1]; // no standard library, no log here
//            auto w = _min;
//            const auto initRoot = _min;
//            do {
//                auto x = w;
//                w = w->_right;
//                auto d = x->_degree;
//                while (!a[d]){
//                    auto y = a[d];
//                    if (FibonacciNode<TKey>::CompareNode(x, y) > 0){
//                        auto t = x;
//                        x = y;
//                        y = t;
//                    }
//                    Link(y, x);
//                    a[d] = nullptr;
//                    ++d;
//                }
//                a[d] = x;
//            } while (w != initRoot);
////            _min = 0;
////            for (int i=0; i<=_degree; ++i) {
////                if (!a[i]){
////                    if (!_min){
////                        AddToRootList(a[i]);
////                        _min=a[i];
////                    } else {
////                        AddToRootList(a[i]);
////                        if (CompareKey(a[i]->GetKey(), _min->GetKey()))
////                            _min = a[i];
////                    }
////                }
////            }
//            _min = w;
//
//        }
//
//        void Link(SharedPointer<FibonacciNode<TKey>> y, SharedPointer<FibonacciNode<TKey>> x){
//            RemoveFromRootList(y);
//            x->AddChild(y);
//            y->_mark = false;
//        }
//
//        void Cut(SharedPointer<FibonacciNode<TKey>> x, SharedPointer<FibonacciNode<TKey>> y){
//            y->DeleteChild(x);
//            AddToRootList(x);
//            x->_parent = 0;
//            x->_mark = false;
//        }
//
//        void CascadingCut(SharedPointer<FibonacciNode<TKey>> y){
//            auto z = y->_parent.Pin();
//            if (!z){
//                if (!y->_mark)
//                    y->_mark = true;
//                else{ // node y has lost more than two children before being cut last time
//                    // therefore it's not balanced and need to be cut
//                    Cut(y, z);
//                    CascadingCut(z);
//                }
//            }
//        }
//
//        static int D(int input) {
//
//        }
//
//    public:
//        FibonacciHeap(SharedPointer<IComparator<TKey>> comparator): _min(nullptr), _degree(0), _comparator(comparator) {
//        }
//
//        inline bool CompareKey(TKey lhs, TKey rhs) const {
//            return _comparator->Invoke(lhs, rhs);
//        }
//
//        SharedPointer<FibonacciNode<TKey>> Insert(TKey key){
//            if (!_comparator)
//                cout << "comparator is null\n";
//            if (!ManagedObject<FibonacciHeap<TKey>>::SharedPointerToThis()) {
//                cout << "self.this is null\n";
//            }
//            auto node = MakeShared<FibonacciNode<TKey>>(key, ManagedObject<FibonacciHeap<TKey>>::SharedPointerToThis());
//            if (!_min) {
//                AddToRootList(node);
//                _min = node;
//            } else {
//                AddToRootList(node);
//                if (FibonacciNode<TKey>::CompareNode(node, _min) < 0)
//                    _min = node;
//            }
//            ++_degree;
//            return node;
//        }
//
//        static SharedPointer<FibonacciHeap<TKey>> Union(SharedPointer<FibonacciHeap<TKey>> a, SharedPointer<FibonacciHeap<TKey>> b){
//            ThrowIf0(a);
//            ThrowIf0(b);
//            if (a->_comparator != b->_comparator)
//                ShallThrow("Argument trees are of different comparators");
//            auto ans = MakeShared<FibonacciHeap<TKey>>(a->_comparator);
//            ans._min = a->_min;
//            b->GetChildren()->ForEach([a](FibonacciNode<TKey> child) { a->AddToRootList(child); });
//            if (!a->_min || (!b->_min && FibonacciNode<TKey>::CompareNode(b, a) < 0))
//                ans->_min = b->_min;
//            ans._degree = a._degree + b._degree;
//            return ans;
//        }
//
//        SharedPointer<FibonacciNode<TKey>> GetMinimum() const { return _min; }
//        SharedPointer<FibonacciNode<TKey>> ExtractMin(){
//            const auto z = _min;
////            if (!z){
////                auto _lam_addToRoot = [this](SharedPointer<FibonacciNode<TKey>> node) { AddToRootList(node); node->_parent = 0; };
////                z->GetChildren()->ForEach(LambdaToFunctor<decltype(_lam_addToRoot), void, SharedPointer<FibonacciNode<TKey>>>(_lam_addToRoot));
////            }
////            RemoveFromRootList(z);
//            if (z == z->_right)
//                _min = 0;
//            else{
//                _min = z->_right;
//                Consolidate();
//            }
//            return z;
//        }
//
//    private:
//        void DecreaseKeyImpl(SharedPointer<FibonacciNode<TKey>> x){
//            const auto y = x->_parent.Pin();
//            if (!y && FibonacciNode<TKey>::CompareNode(x, y) < 0){
//                Cut(x, y);
//                CascadingCut(y);
//            }
//            if (FibonacciNode<TKey>::CompareNode(x, _min) < 0)
//                _min = x;
//        }
//
//    public:
//        void DecreaseKey(SharedPointer<FibonacciNode<TKey>> x, TKey k){
//            // safe to assume that right now no nodes are being marked as minimum
//            // unless the heap is operated through multiple threads, which shall be externally protected
//            if (CompareKey(k, x->_key) >= 0)
//                ShallThrow("key value no less than the original key value");
//            x->_key = k;
//            DecreaseKeyImpl(x);
//        }
//
//        void Delete(SharedPointer<FibonacciNode<TKey>> x){
//            x->_minimumCausedByDeletion = true;
//            DecreaseKeyImpl(x);
//            x->_minimumCausedByDeletion = false;
//            ExtractMin();
//        }
//
//        int GetCount() const override {
//            return _degree;
//        }
//        void Add(TKey key) override { Insert(key); }
//        void Clear() override {
//            _degree = 0;
//            _min = 0;
//        }
//
//        ~FibonacciHeap() override {
//            if (_min) {
//                _min->DestroyRing();
//            }
//        }
//    };
//
//    template <class TKey>
//    SharedPointer<FibonacciHeap<TKey>> MakeHeapWithDefaultConstructor()
//    #if ENABLECONCEPT
//    requires (CharTraits<TKey>::Value||IsIntegral<TKey>::Value)
//    #endif
//    {
//        return MakeShared<FibonacciHeap<TKey>>(GetDefaultComparator<TKey>());
//    }
//}
//

#include "Math.h"

namespace Commons::Collections {
    template<class TKey, class TValue>
    class FibHeap;
    namespace __impl {
        template<class TKey, class TValue>
        class _FibHeapTraverse;
    }

    template<class TKey, class TValue = TKey>
    class FibNode :public ManagedObject<FibNode<TKey, TValue>> {
    private:
        using Node = FibNode<TKey, TValue>;
        friend class FibHeap<TKey, TValue>;
        friend class __impl::_FibHeapTraverse<TKey, TValue>;

        TKey key;
        TValue value;
        int degree;
        WeakPointer <Node> left;
        SharedPointer <Node> right;
        SharedPointer <Node> child;
        WeakPointer <Node> parent;
        bool marked;
        bool minimumMarkDuringRemove;

    public:
        FibNode(TKey key, TValue value) :key(key), value(value), degree(0), marked(false), left(nullptr), right(nullptr), child(nullptr), parent(nullptr), minimumMarkDuringRemove(false) { }
        FibNode(TKey key) :FibNode(key, key) { }

        TKey Key() const { return key; }
        TValue Value() const { return value; }
        SharedPointer<Node> Left() const { return left.Pin(); }
        SharedPointer<Node> Right() const { return right; }
        SharedPointer<Node> Parent() const { return parent.Pin(); }
    };

    template<class TKey, class TValue = TKey>
    class FibHeap :public ManagedObject<FibHeap<TKey, TValue>>, public IKeyValueCollection<TKey, TValue> {
    private:
        using Node = FibNode<TKey, TValue>;
        using Shared = SharedPointer<Node>;
        using Weak = WeakPointer<Node>;
        using EnumerateType = SharedPointer<Pair<TKey, TValue>>;

        int num;
        Shared min;
        SharedPointer<IValueComparator<TKey>> comparator;

    public:
        FibHeap(SharedPointer<IValueComparator<TKey>> c) :num(0), comparator(c), min(nullptr) { }

    private:
        int CompareKey(TKey a, TKey b) const {
            return comparator->Invoke(a, b);
        }
        int CompareNode(Shared a, Shared b) const {
            if (a->minimumMarkDuringRemove)
                return -1;
            if (b->minimumMarkDuringRemove)
                return 1;
            return CompareKey(a->key, b->key);
        }

        void RemoveNode(Shared node) {
            node->Left()->right = node->right;
            node->right->left = node->Left();
        }

        void AddNode(Shared node, Shared root) {
            node->left = root->Left();
            node->Left()->right=node;
            node->right=root;
            root->left=node;
        }
        void Insert(Shared node) {
            if (num == 0) {
                min = node;
                node->left = node;
                node->right = node;
            }
            else {
                AddNode(node, min);
                if(node->key < min->key)
                    min = node;
            }
            ++num;
        }

        void ConcatenateRootList(Shared a, Shared b) {
            Shared t;
            t=a->right;
            a->right=b->right;
            b->right->left=a;
            b->right=t;
            t->left=b;
        }

        void Combine(SharedPointer<FibHeap<TKey, TValue>> other) {
            ThrowIf0(other);
            if (!min) {
                min=other->min;
                num = other->num;
            }
            else if (!other->min) {
            }
            else {
                ConcatenateRootList(min, other->min);
                if (CompareKey(min->key, other->min->key) > 0) {
                    min = other->min;
                }
                num += other->num;
            }
        }

        void Link(Shared node, Shared root) {
            RemoveNode(node);
            if (!root->child)
                root->child = node;
            else
                AddNode(node, root->child);
            node->parent = root;
            ++root->degree;
            node->marked = false;
        }
        void Consolidate() {
            auto log = &Commons::Math::MathFunctionProvider<int>::Log;
            int D = log(num)/log(2) + 1;
            Shared a[D+1];
            while (min) {
                Shared x = ExtractMin();
                int d = x->degree;
                while (a[d]) {
                    auto y = a[d];
                    if (CompareNode(x, y) > 0)
                        Swap(x, y);
                    Link(y, x);
                    a[d] = nullptr;
                    ++d;
                }
                a[d] = x;
            }
            min = nullptr;
            for (int i=0; i<D; ++i) {
                if (!a[i]) {
                    if (!min)
                        min = a[i];
                    else {
                        AddNode(a[i], min);
                        if (CompareNode(a[i], min) < 0)
                            min = a[i];
                    }
                }
            }
        }
        void RemoveMin(){
            if (!min)
                return;
            Shared child = nullptr;
            Shared m = min;
            while (m->child) {
                child = m->child;
                RemoveNode(child);
                if (child->right == child)
                    m->child = nullptr;
                else
                    m->child = child->right;
            }
            RemoveNode(m);
            if (m->right == m)
                min = nullptr;
            else {
                min = m->right;
                Consolidate();
            }
            --num;
        }

        void RefreshDegree(Shared parent, int degree){
            parent->degree -= degree;
            if (!parent->parent)
                RefreshDegree(parent->parent, degree);
        }
        void Cut(Shared node, Shared parent) {
            RemoveNode(node);
            RefreshDegree(parent, node->degree);
            if (node == node->right)
                parent->child = nullptr;
            else
                parent->child = node->right;
            node->parent = nullptr;
            node->left = node->right = node;
            node->marked = false;
            AddNode(node, min);
        }
        void CascadingCut(Shared node) {
            auto parent = node->Parent();
            if (!parent) {
                if (!node->marked)
                    node->marked = true;
                else {
                    Cut(node, parent);
                    CascadingCut(parent);
                }
            }
        }

        Shared Find(Shared root, TKey key) const {
            auto t = root;
            Shared p = nullptr;
            if (!root)
                return root;
            do {
                if (CompareKey(t->key, key) == 0) {
                    p = t;
                    break;
                } else {
                    if (!(p = Find(t->child, key)))
                        break;
                }
                t = t->right;
            } while (t != root);
            return p;
        }

        void Remove(Shared node) {
            node->minimumMarkDuringRemove = true;
            RemoveMin();
        }

        void BreakChain(Shared node) {
            if (!node)
                return;
            const auto start = node;
            do {
                BreakChain(node->child);
                node = node->right;
            } while (node != start);
        }

    public:
        Shared Insert(TKey key, TValue value) {
            auto node = MakeShared<Node>(key, value);
            Insert(node);
            return node;
        }

        Shared ExtractMin() {
            Shared p = min;
            if (p == p->right)
                min = nullptr;
            else {
                RemoveNode(p);
                min = p->right;
            }
            p->left=p->right=p;
            return p;
        }

        Shared Minimum() const {
            return min;
        }

        void DecreaseKey(Shared node, TKey key) {
            if (!min || !node)
                DummyThrow();
            if (CompareKey(key, node->key) > 0) {
                DummyThrow();
            }
            node->key = key;
            auto parent = node->Parent();
            if (!parent && CompareNode(node, parent) < 0) {
                Cut(node, parent);
                CascadingCut(parent);
            }
            if (CompareNode(node, min) < 0)
                min = node;
        }
        void IncreaseKey(Shared node, TKey key) {
            if (!min || !node)
                DummyThrow();
            if (CompareKey(key, node->key) <= 0)
                DummyThrow();
            while (node->child) {
                auto child = node->child;
                RemoveNode(child);
                if (child->right == child)
                    node->child = nullptr;
                else
                    node->child = child->right;
                AddNode(child, min);
                child->parent = nullptr;
            }
            node->degree = nullptr;
            node->key = key;
            auto parent = node->Parent();
            if (!parent) {
                Cut(node, parent);
                CascadingCut(parent);
            }
            else if (min == node) {
                auto right = node->right;
                while (right != node) {
                    if (CompareNode(node, right) > 0)
                        min = right;
                    right = right->right;
                }
            }
        }

        void UpdateKey(TKey oldKey, TKey newKey) {
            auto node = Find(oldKey);
            if (!node)
                DummyThrow();
            Update(node, newKey);
        }

        void Add(TKey key, TValue value) override {
            Insert(key, value);
        }

        SharedPointer<IEnumerator<Pair<TKey, TValue>>> GetEnumerator() const override {
            return MakeShared<__impl::_FibHeapTraverse<TKey, TValue>>(this).template StaticCast<IEnumerator<Pair<TKey, TValue>>>();
        }

        int Count() const override {
            return num;
        }

        SharedPointer<TValue> Find(TKey key) const override {
            if (!min)
                return nullptr;
            auto p = Find(min, key);
            if (!p)
                return nullptr;
            return MakeShared<TValue>(p->value);
        }

        bool Contains(TKey key) const override {
            return Find(key);
        }

        void Remove(TKey key) override {
            if (!min)
                return;
            auto node = Find(key);
            if (!node)
                return;
            Remove(node);
        }

        void Clear() override {
            BreakChain(min);
            min = nullptr;
            num = 0;
        }

        ~FibHeap() noexcept override {
            BreakChain(min);
        }
    };

    namespace __impl {
        template <class TKey, class TValue>
        class _FibHeapTraverse : public IEnumerator<Pair<TKey, TValue>> {
        private:
            typedef FibNode<TKey, TValue> Node;
            typedef Pair<TKey, TValue> EnumerateType;
            typedef SharedPointer<Pair<TKey, TValue>> RetType;

            int state;
            SharedPointer<List<EnumerateType>> a;
            SharedPointer<IEnumerator<EnumerateType>> em;
            SharedPointer<Node> tree_min;

            void AddToList(SharedPointer<Node> node) {
                EnumerateType f(node->key, node->value);
                a->Add(f);
            }
            void Traverse(SharedPointer<Node> node) {
                auto start = node;
                if (!node)
                    return;
                do {
                    AddToList(node);
                    Traverse(node->child);
                    node = node->right;
                } while (node != start);
            }
            void TraverseAll() {
                a = SharedPointer<List<EnumerateType>>();
                if (!tree_min)
                    return;
                auto p = tree_min;
                do {
                    AddToList(p);
                    Traverse(p->child);
                    p = p->right;
                } while (p != tree_min);
                em = a->GetEnumerator();
                a = nullptr;
            }

        public:
            _FibHeapTraverse(const FibHeap<TKey, TValue>* heap): state(0), a(nullptr), tree_min(heap->Minimum()) {
            }

            bool MoveNext() noexcept override {
                if (state == 0) {
                    TraverseAll();
                    state = 1;
                }
                return em->MoveNext();
            }
            RetType Current() const noexcept override {
                return em->Current();
            }
        };
    }

    template <class TKey>
    class PriorityQueue: private FibHeap<TKey, TKey>, public Queue<TKey> {
    public:
        using FibHeap<TKey, TKey>::FibHeap;
        PriorityQueue(IValueComparator<TKey> c): FibHeap<TKey, TKey>(c) {}
        void Enqueue(TKey key) override {
            FibHeap<TKey, TKey>::Insert(key, key);
        }
        TKey Front() const override {
            return FibHeap<TKey, TKey>::Minimum()->Value();
        }
        TKey Dequeue() override {
            return FibHeap<TKey, TKey>::ExtractMin()->Value();
        }
    };
}