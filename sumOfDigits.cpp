# include <iostream> 
using namespace std; 

int sumOfDigits(int num){
    int sum = 0; 
    while (num > 0){
        int digit = num % 10; // remainder 
        sum += digit; 
        num = num / 10;
    }
    return sum;
}

int main(){
    // Calcuate the sum of digits of a number 
    int num; 
    cout << "Enter a number: " << endl;
    cin >> num;

    cout << "Sum of digits: " << sumOfDigits(num) << endl;
    return 0;
}