# include <iostream>
using namespace std;

// calc sum of nos from 1 to n
int calcSum(int n){
    int totalSum = 0;
    for (int i = 0; i <= n; i ++){
        totalSum += i;
    }
    return totalSum;
}
// calc N factorial 
int calcFactorial(int n){
    // n! = n * (n - 1)!
    int fact = 1;
    for (int i = n; i > 0; i --){
        fact *= i;
    }
    return fact;
}
// sum of digits of a number 
int sumOfDigits(int n){
    int totalSum = 0; 

    while (n != 0){
        int remainder = n % 10 ;
        totalSum += remainder;
        n = n / 10;
    }
    return totalSum;
}
// calculate nCr binomial coefficient for n & r 
int binomial(int n, int r){
    // nCr = n!/(r! * (n - r)!)
    int factN = calcFactorial(n);
    int factR = calcFactorial(r);
    int factNr = calcFactorial(n - r); 
    int result = (factN) / (factR * (factNr));
    return result;

}

int main(){
    // calc sum of nos from 1 to n
    cout << " Sum = " << calcSum(10) << endl;
    cout << " Factorial = " << calcFactorial(4) << endl;
    cout << " sum = " << sumOfDigits(2345) << endl;
    cout << " Binomial = " << binomial(8, 2) << endl;
    return 0;
}