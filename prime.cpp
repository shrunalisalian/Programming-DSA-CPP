#include <iostream>
using namespace std;

int main(){
    int num; 
    cout << "Enter the number "; 
    cin >> num;
    bool isPrime = true; // assuming the num is always prime

    if (num == 1){
        isPrime = false; // 0 and 1 are not prime nos
    }else{
        for (int i = 2; i < num ; i ++){
            if (num % i == 0){
                isPrime = false;
                break;
            }
        }
    }
   
    if (isPrime){
        cout << "Prime number " << endl;

    }else{
        cout << "Non prime number " << endl;
    }
    return 0;
}
