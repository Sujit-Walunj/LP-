#include<iostream>   // For standard I/O
#include<stdlib.h>   // For dynamic memory allocation
#include<omp.h>      // For OpenMP parallel programming
using namespace std;

// Function declarations
void mergesort(int a[], int i, int j);                      // Recursive merge sort
void merge(int a[], int i1, int j1, int i2, int j2);        // Merge two sorted subarrays

// Recursive function to perform merge sort
void mergesort(int a[], int i, int j) {
    int mid;

    // Base condition: more than one element
    if (i < j) {
        mid = (i + j) / 2;

        /*
         * Parallel sections: the two recursive calls to `mergesort` can be executed in parallel
         * if they are placed in different sections.
         * OpenMP will run them using multiple threads if available.
         */
        #pragma omp parallel sections
        {
            // One section of recursive merge sort on left half
            #pragma omp section
            mergesort(a, i, mid);

            // Another section on right half
            #pragma omp section
            mergesort(a, mid + 1, j);
        }

        // Merge the two sorted halves
        merge(a, i, mid, mid + 1, j);
    }
}

// Merge two sorted parts of the array: a[i1..j1] and a[i2..j2]
void merge(int a[], int i1, int j1, int i2, int j2) {
    int temp[1000]; // Temporary array to hold merged result
    int i = i1, j = i2, k = 0;

    // Merge while elements remain in both halves
    while (i <= j1 && j <= j2) {
        if (a[i] < a[j]) {
            temp[k++] = a[i++];
        } else {
            temp[k++] = a[j++];
        }
    }

    // Copy remaining elements from left half (if any)
    while (i <= j1) {
        temp[k++] = a[i++];
    }

    // Copy remaining elements from right half (if any)
    while (j <= j2) {
        temp[k++] = a[j++];
    }

    // Copy merged elements back into original array
    for (i = i1, j = 0; i <= j2; i++, j++) {
        a[i] = temp[j];
    }
}

int main() {
    int *a, n, i;

    // Input number of elements
    cout << "\nEnter total number of elements: ";
    cin >> n;

    // Dynamically allocate memory for array
    a = new int[n];

    // Input elements
    cout << "\nEnter elements: ";
    for (i = 0; i < n; i++) {
        cin >> a[i];
    }

    // Get start time
    double start = omp_get_wtime();

    // Call parallel merge sort
    mergesort(a, 0, n - 1);

    // Get stop time
    double stop = omp_get_wtime();

    // Output sorted array
    cout << "\nSorted array is:\n";
    for (i = 0; i < n; i++) {
        cout << a[i] << endl;
    }

    // Print total time taken
    cout << "\nTime taken: " << stop - start << " seconds\n";

    // Free dynamically allocated memory
    delete[] a;
    return 0;
}
