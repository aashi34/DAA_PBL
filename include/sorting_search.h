#ifndef SORTING_SEARCH_H
#define SORTING_SEARCH_H

// ============================================================
// SORTING ALGORITHMS
// ============================================================

// Merge Sort: sorts arr[left..right]
void mergeSort(int arr[], int left, int right);

// Quick Sort: sorts arr[low..high]
void quickSort(int arr[], int low, int high);


// ============================================================
// SEARCHING ALGORITHMS
// Binary search needs a SORTED array.
// Both return the index of target, or -1 if not found.
// ============================================================

int iterativeBinarySearch(int arr[], int size, int target);

int recursiveBinarySearch(int arr[], int left, int right, int target);

#endif
