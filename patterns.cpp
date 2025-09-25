# include <iostream>
using namespace std;

int main(){
    // int n = 4;
    // for (int i = 0; i < n; i ++){
    //     for (int j = 0; j < n; j ++){
    //         cout << (char)(j + 65) << " " ;
    //     }
    //     cout << endl; // prints a newline after each row 
    // }

    // int num = 1; 
    // for (int i = 0; i < n; i ++){
    //     for (int j = 0; j < n; j ++){
    //         cout << (char)(num + 64) << " "; 
    //         num ++;
    //     }
    //     cout << endl;
    //}

    // triangle pattern 
    // int n = 4;
    // for (int i = 1; i <= n; i ++){
    //     for (int j = i; j >= 1; j --){
    //         cout << j << " ";
    //     }
    //     cout << endl;
    // }

    // Floyd's triangle pattern 
    // int n = 3;
    // int num = 1;
    // for (int i = 0; i <= n; i ++){
    //     for (int j = 0; j <= i; j ++){
    //         cout << (char)(num + 64) << " ";
    //         num ++;
    //     }
    //     cout << endl;
    // }

    // Inverted triangle pattern 
    // int n = 4; 
    // for (int i = 0; i < n ; i ++){
    //     for (int j = 0; j < i; j ++){
    //         cout << " ";
    //     }
    //     for (int k = 0; k < n - i; k ++){
    //         cout << i + 1;
    //     }
    //     cout << endl;
    // }

    // Pyramid Pattern 
    // space + 4 outer loops + 3 outer loops 
    // int n = 4; 
    // for (int i = 0;i < n; i ++){
    //     // spaces 
    //     for(int j = 0; j < n - i - 1; j ++){
    //         cout << " ";
    //     }
    //     // numbers
    //     int num = 1; 
    //     for (int k = 0; k <= i; k ++){
    //         cout << num ; 
    //         num ++;
    //     }
    //     // reverse numbers 
    //     for (int a = i; a > 0; a --){
    //         cout << a; 
    //     }
    //     cout << endl;

    // }

    // diamond 
    int n = 4; 
    // top 
    for (int i = 0; i < n; i ++){
        // spaces 
        for (int j = 0; j < n - i - 1; j ++){
            cout << " ";
        }
        cout << "*";
        if (i!= 0){
            // spaces 
            for (int j = 0; j < 2 * i - 1; j ++){
                cout << " ";
            }
            cout << "*";
        }
        cout << endl;
    }
    // bottom 
    for (int i = 0; i < n - 1; i ++){
        // leading spaces (i + 1)
        for (int j = 0; j < i + 1; j ++){
            cout << " ";
        }
        cout << "*";
        if (i != n - 2){ // all rows except the very last one
            // inner spaces 
            for (int j = 0; j < 2 *(n - i - 2) - 1; j++){
                cout << " ";
            }
            cout << "*";
        }
        cout << endl;
    }
    
    return 0;
}   