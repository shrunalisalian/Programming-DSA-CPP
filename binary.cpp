# include <iostream>
using namespace std;

int decToBinary(int dec){
    int answer = 0; // final answer
    int power = 1; // 10^0 -> 10^1 -> 10^2
    while (dec > 0){
        int remainder = dec % 2; // 
        dec = dec / 2; 
        answer += remainder * power; 
        power *= 10;
    }
    return answer;
}
int binaryTodec(int binary){
    int answer = 0; 
    int power = 1; 
    while (binary > 0){
        int remainder = binary % 10; 
        answer += remainder * power; 
        power *= 2; 
        binary /= 10;
    }
    return answer;
}

int main(){
    // decimal to binary number conversion 
    int dec; 
    cout << "Enter decimal number for conversion to binary: "; 
    cin >> dec;
    cout << "Binary equivalent: " << decToBinary(dec) << endl;

    // binary to decimal conversion
    int binary;
    cout << "Enter binary number for conversion to decimal: "; 
    cin >> binary;
    cout << "Decimal equivalent: " << binaryTodec(binary) << endl;
    return 0;
} 
