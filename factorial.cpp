# include <iostream>
using namespace std; 

int factorial(int n){
    int fact = 1; // fact(0) = 1; fact(1) = 1
    // fact(n) = fact(n - 1) * n 
    int i = n;
    while (i > 0){
        fact = fact * i; 
        i--;
    }
    cout << "Factorial (" << n << ") = " << fact << endl;
    return fact;
}

int factProduct(int n){
    if (n == 0 || n == 1){
        return 1;
    }
    int factorial = factProduct(n - 1) * n;
    return factorial;
}

int main(){
    factorial(5);
    int result = factProduct(5);
    cout << "Factorial using recursion = " << result << endl;
    return 0;
}