# include <iostream>
# include <vector>
using namespace std;

// Brute Force Approach - O(n^2)
int bruteForce(vector <int> &height){
    int maxWater = 0;
    // left & right boundary
    for (int left = 0; left < height.size(); left++){
        // for every left boundary get a right boundary and calc maxWater
        for (int right = left + 1; right < height.size(); right++){
            // calc max water 
            maxWater = max(maxWater, (right - left)* min(height[left], height[right]));
        }
    }
    return maxWater;
}
// optimal solution - O(n)
int optimized(vector <int> &height){
    int maxWater = 0;
    int left = 0, right = height.size() - 1;
    while (left < right){
        maxWater = max(maxWater, (right - left) * min(height[left], height[right]));
        if (height[left] < height[right]){
            left++;
        }else{
            right --;
        }
    }
    return maxWater;
}

int main(){
    vector <int> height = {1,8,6,2,5,4,8,3,7};
    cout << "Container with max water (brute force): " << bruteForce(height) << endl; 
    cout << "Container with max water (2 pointers): " << optimized(height) << endl; 
    return 0;
}