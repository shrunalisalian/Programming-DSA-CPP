# include <iostream>
using namespace std; 

int largest(int a, int b, int c){
    int result; 
    if (a > b && a > c){
        result = a;
    }else if (b > c){
        result = b;
    }else{
        result = c;
    }
    return result;
}

int main(){
    int a, b, c; 
    cout << "Enter 3 numbers" << endl;
    cin >> a >> b >> c; 

    cout << "Largest out of the three: " << largest(a, b, c) << endl;
}