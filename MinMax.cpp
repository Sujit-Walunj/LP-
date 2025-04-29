#include <iostream>     // For standard input/output
#include <vector>       // For using vectors (though not used here)
#include <omp.h>        // For OpenMP parallelization
#include <climits>      // For INT_MAX and INT_MIN
using namespace std;

/*
 * The reduction clause in OpenMP is used to avoid data races when performing
 * operations like min, max, sum, etc., across multiple threads.
 * Each thread gets a private copy of the variable, and OpenMP combines the results safely.
 */

// Find minimum value using OpenMP reduction
void min_reduction(int arr[], int n) {
    int min_value = INT_MAX;  // Initialize to maximum possible integer

    #pragma omp parallel for reduction(min: min_value)
    for (int i = 0; i < n; i++) {
        if (arr[i] < min_value) {
            min_value = arr[i];  // Update if smaller value found
        }
    }

    cout << "Minimum value: " << min_value << endl;
}

// Find maximum value using OpenMP reduction
void max_reduction(int arr[], int n) {
    int max_value = INT_MIN;  // Initialize to minimum possible integer

    #pragma omp parallel for reduction(max: max_value)
    for (int i = 0; i < n; i++) {
        if (arr[i] > max_value) {
            max_value = arr[i];  // Update if larger value found
        }
    }

    cout << "Maximum value: " << max_value << endl;
}

// Calculate sum using OpenMP reduction
void sum_reduction(int arr[], int n) {
    int sum = 0;

    #pragma omp parallel for reduction(+: sum)
    for (int i = 0; i < n; i++) {
        sum += arr[i];  // Accumulate sum
    }

    cout << "Sum: " << sum << endl;
}

// Calculate average using OpenMP reduction
void average_reduction(int arr[], int n) {
    int sum = 0;

    #pragma omp parallel for reduction(+: sum)
    for (int i = 0; i < n; i++) {
        sum += arr[i];  // Accumulate sum
    }

    // NOTE: The division should be by 'n', not 'n - 1', unless intentionally excluding one element.
    cout << "Average: " << (double)sum / n << endl;
}

int main() {
    int *arr, n;

    // Input size of array
    cout << "\nEnter total number of elements => ";
    cin >> n;

    // Dynamically allocate memory
    arr = new int[n];

    // Input elements
    cout << "\nEnter elements => ";
    for (int i = 0; i < n; i++) {
        cin >> arr[i];
    }

    // Record start time
    auto start = omp_get_wtime();

    // Perform reduction operations
    min_reduction(arr, n);
    max_reduction(arr, n);
    sum_reduction(arr, n);
    average_reduction(arr, n);

    // Record end time
    auto end = omp_get_wtime();

    // Display total execution time
    cout << "Total time required is: " << end - start << " seconds" << endl;

    // Clean up dynamic memory
    delete[] arr;
    return 0;
}
