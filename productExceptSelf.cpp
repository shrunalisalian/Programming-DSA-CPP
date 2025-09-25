# include <iostream>
# include <vector>
using namespace std;

// brute force approach - O(n^2) 
// nums = [-1,1,0,-3,3]
vector <int> bruteForce(vector <int> &nums){
    vector <int> result;
    // for every element we can find the product of all the other elements in the array - O(n^2)
    for (int i = 0; i < nums.size(); i++){
        int product = 1; 
        for (int j = 0; j < nums.size(); j ++){
            if (i != j){
                product *= nums[j];
            }
        }
        result.push_back(product);
    }
    return result;
}

// tc : O(1)  && sc: O(n) approach 
// left product * right product = answer 
vector <int> prefixSuffix(vector <int> &nums){
    vector <int> result (nums.size(), 1);
    vector <int> prefix (nums.size(), 1);
    vector <int> suffix (nums.size(), 1);

    // calculate prefix array 
    for (int i = 1; i < nums.size(); i++){
        prefix[i] *= (prefix[i-1] * nums[i - 1]);
    }
    // calculate suffix array 
    for (int i = nums.size() - 2; i >= 0; i-- ){
        suffix[i] *= (suffix[i + 1] * nums[i + 1]);
    }
    // calculate final result 
    for (int i = 0; i < nums.size(); i++){
        result[i] = prefix[i] * suffix[i];
    }

    return result;
}

// sc: O(1) approach 
// instead of storing and later using prefix value for multiplication why don't you directly multiply
vector <int> spaceOptimised(vector <int> &nums){
    vector <int> result (nums.size(), 1);
    int prefix = 1, suffix = 1; 

    // calculate prefix 
    for (int i = 1; i < nums.size(); i++){
        // prefix *= result[i - 1];
        result[i] = (result[i - 1] * nums[i - 1]);
    }
    // calculate suffix 
    for (int i = nums.size() - 2; i >= 0; i--){
        suffix *= nums[i + 1];
        // we found the suffix. Now we can directly multiply it with the result array 
        result[i] *= suffix;
    }
    return result;
}


int main(){
    vector <int> nums = {-1,1,0,-3,3};
    
    cout << "Product except self (brute force): ";
    vector <int> result = bruteForce(nums);
    for (int val: result){
        cout << val << " ";
    }
    cout << endl;

    cout << "Product except self (time optimised): ";
    vector <int> time_optimised = prefixSuffix(nums);
    for (int val: time_optimised){
        cout << val << " ";
    }
    cout << endl;

    cout << "Product except self (space optimised): ";
    vector <int> space_optimised = spaceOptimised(nums);
    for (int val: space_optimised){
        cout << val << " ";
    }
    cout << endl;


    return 0;
}