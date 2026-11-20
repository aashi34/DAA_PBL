#include "../include/sorting_search.h"


// ============================================================
// MERGE SORT
// Time: O(n log n)   Auxiliary space: O(n)
//
// Uses one temporary array. It can sort up to 100000 numbers.
// ============================================================

const int MAX_SORT_SIZE = 100000;

static int tempArray[MAX_SORT_SIZE];


// Merge the two sorted halves arr[left..middle] and arr[middle+1..right]
static void mergeArrays(int arr[], int left, int middle, int right)
{
    // Copy the part we are merging into the temporary array
    for (int x = left; x <= right; x++)
    {
        tempArray[x] = arr[x];
    }

    int i = left;           // position in the left half
    int j = middle + 1;     // position in the right half
    int k = left;           // position in arr

    // Take the smaller of the two front elements each time
    while (i <= middle && j <= right)
    {
        if (tempArray[i] <= tempArray[j])
        {
            arr[k] = tempArray[i];
            i++;
        }
        else
        {
            arr[k] = tempArray[j];
            j++;
        }

        k++;
    }

    // Copy what is left in the left half
    while (i <= middle)
    {
        arr[k] = tempArray[i];
        i++;
        k++;
    }

    // Copy what is left in the right half
    while (j <= right)
    {
        arr[k] = tempArray[j];
        j++;
        k++;
    }
}


void mergeSort(int arr[], int left, int right)
{
    if (left >= right)
    {
        return;
    }

    int middle = left + (right - left) / 2;

    mergeSort(arr, left, middle);          // sort left half
    mergeSort(arr, middle + 1, right);     // sort right half
    mergeArrays(arr, left, middle, right); // combine them
}


// ============================================================
// QUICK SORT
// Average: O(n log n)   Worst: O(n^2)
// Average recursion space: O(log n)
// ============================================================

// Put the pivot (last element) in its correct place.
// Smaller elements go to its left, bigger ones to its right.
static int partitionArray(int arr[], int low, int high)
{
    int pivot = arr[high];

    int i = low - 1;

    for (int j = low; j < high; j++)
    {
        if (arr[j] <= pivot)
        {
            i++;

            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }

    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;

    return i + 1;
}


void quickSort(int arr[], int low, int high)
{
    if (low < high)
    {
        int pivotIndex = partitionArray(arr, low, high);

        quickSort(arr, low, pivotIndex - 1);   // left of pivot
        quickSort(arr, pivotIndex + 1, high);  // right of pivot
    }
}


// ============================================================
// ITERATIVE BINARY SEARCH  (array must be sorted)
// Time: O(log n)   Space: O(1)
// ============================================================

int iterativeBinarySearch(int arr[], int size, int target)
{
    int left = 0;
    int right = size - 1;

    while (left <= right)
    {
        int middle = left + (right - left) / 2;

        if (arr[middle] == target)
        {
            return middle;
        }

        if (arr[middle] < target)
        {
            left = middle + 1;      // look in the right half
        }
        else
        {
            right = middle - 1;     // look in the left half
        }
    }

    return -1;      // not found
}


// ============================================================
// RECURSIVE BINARY SEARCH  (array must be sorted)
// Time: O(log n)   Space: O(log n) (recursion)
// ============================================================

int recursiveBinarySearch(int arr[], int left, int right, int target)
{
    if (left > right)
    {
        return -1;      // not found
    }

    int middle = left + (right - left) / 2;

    if (arr[middle] == target)
    {
        return middle;
    }

    if (arr[middle] < target)
    {
        return recursiveBinarySearch(arr, middle + 1, right, target);
    }

    return recursiveBinarySearch(arr, left, middle - 1, target);
}
