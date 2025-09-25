# include <iostream>
# include <vector>
using namespace std;

// print all subarrays in an array
int printSubArray(){
    vector <int> vec = {1, 2, 3, 4, 5};
    for (int start = 0; start < vec.size(); start ++){
        for (int end = start; end < vec.size(); end ++){
            for (int i = start; i <= end; i++){
                cout << vec[i];
            }
            cout << " ";
        }
        cout << endl;
    }
    return 0;
}
// Calculate the maximum subarray sum: [5,4,-1,7,8]
int maximumSubArraySum(vector <int> &vec){
    // vector <int> vec = {5,4,-1,7,8}; 
    int result = INT_MIN; 
    for (int start = 0; start < vec.size(); start ++){
        int currSum = 0;
        for (int end = start; end < vec.size(); end ++){
            currSum += vec[end];
        }
        result = max(result, currSum); 
    }
    return result;
}
// Kadane's Algorithm
int kadaneAlgorithm(vector <int> &vec){
    /*  INTUITION behind Kadane's Algorithm 
    when you are trying to find the max sum for any given array if while calculating the sum 
    your sum becomes less than 0 i.ie, negative better than adding that particular number reset currSum
    to 0 because adding a big negative number cannot give max sum for a particular array.
    */
    int maxSum = INT_MIN, currSum = 0;
    for (int i = 0; i < vec.size(); i++){
        currSum += vec[i];
        maxSum = max(maxSum, currSum);

        if (currSum + vec[i] < 0){
            currSum = 0;
        }
    }
    return maxSum;
}


int main(){
    vector <int> vec = {3,-4,5,4,-1,7,8};
    cout << "Maximum subarray sum = " << maximumSubArraySum(vec) << endl;
    cout << "Maximum subarray sum = " << kadaneAlgorithm(vec) << endl;
    return 0;
}