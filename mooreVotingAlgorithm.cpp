# include <iostream>
# include <vector>
using namespace std;

// 169. Majority Element
int bruteForce(vector <int> &vec){
    for (int val : vec){
        int freq = 0;
        for (int element: vec){
            if (val == element){
                freq ++;
            }
        }
        if (freq > (vec.size()/2)){
            return val;
        }
    }
    // if no element majority
    return -1;
}

int optmization(vector <int> &vec){
    sort(vec.begin(), vec.end()); // 1,1,2,2,2
    int ans = vec[0], freq = 1;

    for (int i = 1; i < vec.size(); i ++){
        if (vec[i] == vec[i - 1]){
            // same element 
            freq ++;
        }else{
            freq = 1;
            ans = vec[i];
        }
        if (freq > vec.size()/2){
            return ans;
        }
    }
    return -1;
}
// Moore's Voting Algorithm
int mooreVoting(vector <int> &vec){
    int freq = 0, cand = 0;
    for (int i = 0; i < vec.size(); i++){
        if (freq == 0){
            cand = vec[i];
        }
        if (cand == vec[i]){
            freq ++;
        }else{
            freq --;
        }
    }
    // If there ans doesnt always exists
    int count = 0;
    for (int val: vec){
        if (val == cand){
            count ++;
        }
    }
    if (count > vec.size()/2){
        return cand;
    }else{
        return -1;
    }
    
}


int main(){
    vector <int> vec = {1,2,3,4,5,6};
    cout << "Majority Element: " << bruteForce(vec) << endl;
    cout << "Majority Element after optmization: " << optmization(vec) << endl;
    cout << "Majority Element after Moore's Voting: " << mooreVoting(vec) << endl;
    return 0;
}