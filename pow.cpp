# include <iostream>
# include <vector>
using namespace std; 
long long binaryExponentiation(int x, int n){
    long long result = 1; 
    // convert the power to its binary equivalent and run the while loop that many times 
    int binForm = n;
    while (binForm > 0){
        int power = binForm % 2;
        if (power == 1){
            result *= x;
        }
        x = x * x;
        binForm /= 2;
    }
    return result;
}
int main(){
    // calculate power : x ^ n
    int x = 3, n = 5;
    cout << " Power of " << x << " to the power " << n << " = " << binaryExponentiation(x, n) << endl;
    return 0;
}