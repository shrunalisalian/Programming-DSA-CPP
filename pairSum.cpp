# include <iostream>
# include <vector>
using namespace std;

// brute force : [2, 7, 11, 15], target = 9
vector <int> bruteForce(vector <int> &vec, int target){
    vector <int> result;
    for (int i = 0; i < vec.size(); i ++){
        for (int j = i + 1; j < vec.size(); j ++){
            if (vec[i] + vec[j] == target){
                result.push_back(i);
                result.push_back(j);
                return result;
            }
        }
    }
    return result;
}

// Using 2 pointer approach 
vector <int> pairSum(vector<int> &vec, int target){
    vector <int> result; 
    int start = 0, end = vec.size() - 1; 
    while (start < end){
        if (vec[start] + vec[end] == target){
            result.push_back(start);
            result.push_back(end);
            return result;
        }else if(vec[start] + vec[end] < target){
            start ++;
        }else{
            end --;
        }        
    }
    return result;
}

int main(){
    vector <int> vec = {2, 7, 11, 15};
    int target = 9;
    vector <int> result = bruteForce(vec, target); 

    cout << "Brute Force Indices: "; 
    for (int idx: result){
        cout << idx << " ";
    }
    cout << endl;

    cout << "Pair Sum approach: ";
    vector <int> pairSumResult = pairSum(vec, target);
    for (int idx: pairSumResult){
        cout << idx << " ";
    }
    cout << endl;
    return 0;
}