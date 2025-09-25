# include <iostream>
using namespace std; 

int reverse(int n){
    int reverseNo = 0; 
    while (n > 0){
        int remainder = n % 10; 
        reverseNo = reverseNo * 10 + remainder;
        n = n / 10; 
    }
    return reverseNo;
}

int main(){
    // check if the given number is a palindrome 
    int n; 
    cout << "Enter a number: " << endl; 
    cin >> n; 
    if (reverse(n) == n){
        cout << "Given number is a palindrome" << endl;
    }else{
        cout << "Given number is not a palindrome" << endl;
    }
    return 0;
}