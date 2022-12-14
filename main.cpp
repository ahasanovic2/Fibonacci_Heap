#include <iostream>
#include <cmath>
#include <vector>
#include <cassert>
#include <ctime>
#include <random>

using namespace std;

template <typename Type>
struct Node {
    int key, degree;
    Node<Type> *parent, *child, *left, *right;
    bool mark, checked;

    Node(Type value) {
        parent = left = child = right = nullptr;
        degree = 0;
        key = value;
        checked = mark = false;
    }
};

template <typename Type>
class FibonacciHeap {
    int n;
    Node<Type>* min;

    Node<Type>* findNode(Node<Type>* y, int k) {
        auto x = y;
        x->checked = true;
        Node<Type>* p = nullptr;

        if (x->key == k) {
            p = x;
            x->checked = false;
            return p;
        }
        Node<Type>* z;
        if (x->child != nullptr)
            z = findNode(x->child, k);
        if (!x->right->checked)
            z = findNode(x->right, k);

        if (z)
            p = z;

        x->checked = false;
        return p;
    }
public:
    FibonacciHeap() {
        n = 0;
        min = nullptr;
    }

    void Insert(Node<Type> *x) {
        x->degree = 0;
        x->parent = nullptr;
        x->child = nullptr;
        x->mark = false;
        if (min == nullptr) {
            x->left = x;
            x->right = x;
            min = x;
        }
        else {
            min->left->right = x;
            x->right = min;
            x->left = min->left;
            min->left = x;
            if (x->key < min->key)
                min = x;
        }
        n++;
    }

    FibonacciHeap Union(FibonacciHeap<Type> h1, FibonacciHeap<Type> h2) {
        FibonacciHeap<Type> h3 = FibonacciHeap<Type>();
        h3.min = h1.min;

        if (h2.min != nullptr && h3.min != nullptr) {
            (h3.min)->left->right = h2.min;
            (h2.min)->left->right = h3.min;
            Node<Type> *np = (h3.min)->left;
            (h3.min)->left = (h2.min)->left;
            (h2.min)->left = np;
        }

        if (h1.min == nullptr || (h2.min != nullptr && (h2.min)->key < (h1.min)->key))
            h3.min = h2.min;

        h3.n = h1.n + h2.n;
        return h3;
    }

    void Consolidate() {
        int D = log2(n) + 1;
        vector<Node<Type>*> A(D, nullptr);
        Node<Type>* x = min;
        do {
            int d = x->degree;
            while (A.at(d) != nullptr) {
                Node<Type>* y = A.at(d);
                if (x->key > y->key) {
                    Node<Type>* b = x;
                    x = y;
                    y = b;
                }
                if (y == min)
                    min = x;
                HeapLink(y, x);
                if (x->right == x)
                    min = x;
                A.at(d) = nullptr;
                d++;
            }
            A.at(d) = x;
            x = x->right;
        }
        while (x != min);

        min = nullptr;

        for (int i = 0; i < D; i++) {
            if (A.at(i) != nullptr) {
                (A.at(i))->left = A.at(i);
                (A.at(i))->right = A.at(i);
                if (min != nullptr) {
                    min->left->right = A.at(i);
                    (A.at(i))->right = min;
                    (A.at(i))->left = min->left;
                    min->left = A.at(i);
                    if ((A.at(i))->key < min->key)
                        min = A.at(i);
                }
                else
                    min = A.at(i);

                if (min == nullptr)
                    min = A.at(i);
                else if ((A.at(i))->key < min->key)
                    min = A.at(i);
            }
        }
    }

    Node<Type>* ExtractMin() {
        Node<Type>* z = min;
        if (z != nullptr) {
            Node<Type>* x = z->child;

            if (x != nullptr) {
                Node<Type> *np, *ptr = x;
                do {
                    np = x->right;
                    min->left->right = x;
                    x->right = min;
                    x->left = min->left;
                    min->left = x;
                    if (x->key < min->key)
                        min = x;
                    x->parent = nullptr;
                    x = np;
                }
                while (np != ptr);
            }

            min->left->right = min->right;
            min->right->left = min->left;

            if (z == z->left && z->child == nullptr)
                min = nullptr;
            else {
                min = z->right;
                Consolidate();
            }
            n--;
        }
        return z;
    }

    void HeapLink(Node<Type>* y, Node<Type>* z) {
        y->left->right = y->right;
        y->right->left = y->left;
        if (z->right == z)
            min = z;

        y->right = y->left = y;
        y->parent = z;

        if (z->child == nullptr)
            z->child = y;

        y->right = z->child;
        y->left = z->child->left;
        z->child->left->right = y;
        z->child->left = y;

        if (y->key < z->child->key)
            z->child = y;

        z->degree++;
    }

    void Display() {
        Node<Type>* p = min;
        if (p == NULL) {
            cout << "Empty heap" << endl;
            return;
        }
        cout << "Root nodes: " << endl;

        do {
            cout << p->key;
            p = p->right;
            if (p != min) {
                cout << "-->";
            }
        } while (p != min && p->right != NULL);
        cout << endl;
    }

    void DecreaseKey(Type x, Type k) {

        if (min == nullptr) {
            cout << "Heap is empty." << endl;
            return;
        }

        Node<Type>* found = findNode(min,k);

        if (!found) {
            cout << "Node is not in heap." << endl;
            return;
        }

        if (found->key < k) {
            cout << "Entered key greater than current key." << endl;
            return;
        }

        found->key = k;
        auto y = found->parent;
        if (y != nullptr && found->key < y->key) {
            Cut(found,y);
            CascadeCut(y);
        }
        if (found->key < min->key)
            min = found;
    }

    void Cut(Node<Type>* x, Node<Type>* y) {
        if (x == x->right)
            y->child = nullptr;
        x->left->right = x->right;
        x->right->left = x->left;

        if (x == y->child)
            y->child = x->right;
        y->degree--;
        x->right = x->left = x;
        min->left->right = x;
        x->right = min;
        x->left = min->left;
        min->left  = x;

        x->parent = nullptr;
        x->mark = false;
    }

    void CascadeCut(Node<Type>* y) {
        auto z = y->parent;
        if (z) {
            if (!y->mark)
                y->mark = true;
            else {
                Cut(y,z);
                CascadeCut(z);
            }
        }
    }

    void DeleteKey(Type k) {
        Node<Type>* np = nullptr;
        DecreaseKey(k, -5000);
        np = ExtractMin();
        if (np)
            cout << "Key deleted" << endl;
        else
            cout << "Key not deleted" << endl;
    }
};

vector<Node<int>*> create_random_elements(int n) {
    // Function that creates vector with N elements in range [0,99];

    default_random_engine defEngine(time(nullptr));
    uniform_int_distribution<int> intDistro(0,100);
    vector<Node<int>*> elements;
    for (int i = 0; i < n; i++) {
        auto help = new Node<int>(intDistro(defEngine));
        elements.emplace_back(help);
    }

    /*cout << "Created elements [0,99] are: ";
    for (int i = 0; i < n-1; i++) {
        cout << (elements.at(i))->key << ", ";
    }
    cout << (elements.at(n-1))->key << endl;*/

    return elements;
}

void delete_elements (const vector<Node<int>*>& elements) {
    for (auto &x: elements) {
        delete x;
    }
    //cout << endl << "Elements are deleted. " << endl;
}

int find_minimum (const vector<Node<int>*>& elements) {
    int min = 100;
    for (auto &x: elements)
        if (x->key < min)
            min = x->key;
    return min;
}

int find_minimum2 (const vector<Node<int>*>& elements1, const vector<Node<int>*>& elements2) {
    int min1 = find_minimum(elements1);
    int min2 = find_minimum(elements2);
    if (min1 > min2)
        return min2;
    return min1;
}

void testInsert1() {
    // This tests random insertion of elements and prints the result.
    cout << "Test Insert 1" << endl;
    cout << endl << "Insert number of elements: ";
    int n;
    cin >> n;

    FibonacciHeap<int> fp;
    auto elements = create_random_elements(n);
    for (auto &x: elements)
        fp.Insert(x);

    //fp.Display();
    assert(1);
    cout << "Test Insert 1 status: PASSED" << endl;

    delete_elements(elements);
}

void testUnion1() {
    // Test unije za 2 napunjene gomile
    cout << endl << "Test Union 1" << endl;
    cout << "Insert number of elements for first heap: ";
    int n;
    cin >> n;
    FibonacciHeap<int> fp;
    auto elements = create_random_elements(n);
    for (auto &x: elements)
        fp.Insert(x);

    cout << endl << "Insert number of elements for second heap: ";
    int m;
    cin >> m;
    FibonacciHeap<int> fp2;
    auto elements2 = create_random_elements(n);
    for (auto &x: elements2)
        fp2.Insert(x);

    FibonacciHeap<int> fp3 = fp.Union(fp, fp2);

    assert(find_minimum2(elements,elements2) == fp3.ExtractMin()->key);
    cout << endl << "Test Union 1 status: PASSED" << endl;

    //fp3.Display();


    delete_elements(elements);
    delete_elements(elements2);
}

void testUnion2() {
    // Test unije za punu i praznu gomilu

    cout << endl << "Test Union 2";
    cout << endl << "Insert number of elements: ";
    int n;
    cin >> n;
    FibonacciHeap<int> fp;
    auto elements = create_random_elements(n);
    for (auto &x: elements)
        fp.Insert(x);

    FibonacciHeap<int> fp2;
    FibonacciHeap<int> fp3 = fp.Union(fp, fp2);

    //fp3.Display();
    assert(find_minimum(elements) == fp3.ExtractMin()->key);
    cout << endl << "Test Union 2 status: PASSED" << endl;
    delete_elements(elements);
}

void testDecreaseKey() {
    cout << "Test Decrease Key 1" << endl;
    auto elements = create_random_elements(15);
    FibonacciHeap<int> fh;
    for (auto &x: elements)
        fh.Insert(x);

    int broj = elements.at(7)->key;



    delete_elements(elements);
}

int main() {
    testInsert1();
    testUnion1();
    testUnion2();
    return 0;
}
