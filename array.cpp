# include <iostream>
using namespace std;

// Linear Search Algorithm: search for 80 in the array return index else return -1
int linearSearch(int array[], int size, int target){
    for (int i = 0; i < size; i ++){
        if (array[i] == target){
            return i;
        }
    }
    return -1;
}
// Reverse an array 
void reverseArray(int array[], int size){
    int start = 0;
    int end = size -1;
    while (start < end){
        swap(array[start], array[end]);
        start ++;
        end --;
    }
    cout << array << endl;
    return;
}

int main(){
    // find the smallest and the largest elements in an int array 
    // find the index of smallest and the largest elements in an int array 

    int smallIdx, largeIdx; 
    int smallest = INT_MAX;
    int array[] = {5, 19, 89, -101, 0};
    int size = sizeof(array) / sizeof(int);

    for (int i = 0; i < size; i++){
        if (smallest > array[i]){
            smallest = array[i];
            smallIdx = i;
        }
    }
    cout << "Smallest:  " << smallest << endl;
    cout << "Smallest Index:  " << smallIdx << endl;
    int largest = INT_MIN;

    for (int i = 0; i < size; i++){
        if (largest < array[i]){
            largest = array[i];
            largeIdx = i;
        }
    }
    cout << "Largest:  " << largest << endl;
    cout << "Largest index:  " << largeIdx << endl;
    int target = 80;
    cout << linearSearch(array, size, target) << endl;

    cout << "Reverse array: " << array << endl;

    for(int i = 0; i < size; i++){
        cout << array[i]<<" " ;
    }
    return 0;
}