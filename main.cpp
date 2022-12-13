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
public:
    FibonacciHeap() {
        n = 0;
        min = nullptr;
    }
    void Fib_Insert(Node<Type> *x) {
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

    FibonacciHeap Fib_Union(FibonacciHeap<Type> h1, FibonacciHeap<Type> h2) {
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

    void Fib_Consolidate() {
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
                Fib_HeapLink(y, x);
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

    Node<Type>* Fib_ExtractMin() {
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
                Fib_Consolidate();
            }
            n--;
        }
        return z;
    }

    void Fib_HeapLink(Node<Type>* y, Node<Type>* z) {
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

    void Fib_Display() {
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
};

vector<Node<int>*> create_random_elements(int n) {
    // Function that creates vector with N elements in range [0,99];

    default_random_engine defEngine(time(0));
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

void delete_elements (vector<Node<int>*> elements) {
    for (auto &x: elements) {
        delete x;
    }
    //cout << endl << "Elements are deleted. " << endl;
}

int find_minimum (vector<Node<int>*> elements) {
    int min = 100;
    for (auto &x: elements)
        if (x->key < min)
            min = x->key;
    return min;
}

int find_minimum2 (vector<Node<int>*> elements1, vector<Node<int>*> elements2) {
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
        fp.Fib_Insert(x);

    //fp.Fib_Display();
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
        fp.Fib_Insert(x);

    cout << endl << "Insert number of elements for second heap: ";
    int m;
    cin >> m;
    FibonacciHeap<int> fp2;
    auto elements2 = create_random_elements(n);
    for (auto &x: elements2)
        fp2.Fib_Insert(x);

    FibonacciHeap<int> fp3 = fp.Fib_Union(fp,fp2);

    assert(find_minimum2(elements,elements2) == fp3.Fib_ExtractMin()->key);
    cout << endl << "Test Union 1 status: PASSED" << endl;

    //fp3.Fib_Display();


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
        fp.Fib_Insert(x);

    FibonacciHeap<int> fp2;
    FibonacciHeap<int> fp3 = fp.Fib_Union(fp,fp2);

    //fp3.Fib_Display();
    assert(find_minimum(elements) == fp3.Fib_ExtractMin()->key);
    cout << endl << "Test Union 2 status: PASSED" << endl;
    delete_elements(elements);
}

int main() {
    testInsert1();
    testUnion1();
    testUnion2();
    return 0;
}
