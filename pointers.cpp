# include <iostream>
# include <vector>
using namespace std;

int main(){
    int a = 50;
    int *ptr1 = &a; 
    int **ptr2 = &ptr1;

    cout << "Memory location of a = " << &a << endl; // address of a 
    cout << "*ptr: Value at memory location ptr " << *ptr1 << endl; // address of a = &a
    cout << "**ptr2: ptr2 -> ptr1 Hence, Value at memory location ptr1 " << **ptr2 << endl; // value stored at (address of (address of ptr1)) = address of a
    cout << "ptr1 - pointer to a. Print value of ptr1 which is memory location of a " << ptr1 << endl; // address of a 
    cout << "*ptr2 - Dereference - Value at memory location ptr2. ptr2 is a pointer to ptr1. Memory Location of ptr1 =  " << *ptr2 << endl; // value stored at address of ptr2 i.e., address of ptr1

    return 0;
}