# include <iostream>
using namespace std; 

int isPrime(int n){
    bool prime = true ; // lets assume that the given integer is prime 
    for (int i = 2; i < n; i++){
        if (n % i == 0){
            prime = false; // no is not prime 
            break;
        }
    }
    if (prime) {
        return n;
    }else{
        return -1;
    }
}

int main(){
    // print all prime numbers in the range 2 to n 
    int n; 
    cout << "Enter the value of n" << endl;
    cin >> n; 
    int result;
    vector <int> resultArray; 
    for (int i = 2; i < n; i++){
        if (isPrime(i) != -1){
            resultArray.push_back(i);
        }
    }

    for (int i; i < resultArray.size(); i++){
        cout << resultArray[i] << " "; 
    }
    cout << endl;
    return 0;
}