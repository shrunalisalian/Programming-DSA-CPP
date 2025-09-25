# include <iostream>
using namespace std; 

void sayHello(){
    cout << "Hello!" << endl;
}

void assistant(){
    sayHello();
}

int main(){
    assistant();
    int i = 5; 
    while (i > 0){
        assistant(); 
        i --;
    }
    return 0;
}