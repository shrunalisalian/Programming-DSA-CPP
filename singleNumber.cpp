# include <iostream>
# include <vector>
using namespace std;
// leetcode 136: Single Number 
int main(){
    int result = 0;
    vector <int> vec = {4,1,2,1,2};
    for (int i = 0; i < vec.size(); i ++){
        result ^= vec[i];
    }
    cout << "Result: " << result << endl;
    return 0;
}