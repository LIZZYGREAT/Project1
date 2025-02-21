#include<iostream>
#include<iomanip>
#include<cmath>
#include<cstdio>
#include<algorithm>
#include<stdio.h>
using namespace std;
int main()
{
    const int max = 10;
    int a[max][max] = { 0 };
    int m, n;
    cin >> m >> n;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cin >> a[i][j];
        }
    }
    for (int i = 0; i < m; i++)
    {       
        if ((i+1) % 2 != 0)
        {
            for (int j = 0; j < n; j++) 
            {
                cout << a[i][j] << " ";
            }
        }
        else 
        {
            for (int j = n - 1; j >= 0; j--) 
            {
                cout << a[i][j] << " ";
            }
        }
    }
    cout << endl;
    int bottom = 1, int right = 1;
    int top = 0, int bottom = m - 1, int left = 0, int right = n - 1;
    while (top <= bottom && left <= right) {
        for (int i = left; i <= right; ++i) {
            cout << a[top][i] << " ";
        }
        top++;
        for (int i = top; i <= bottom; ++i) {
            cout << a[i][right] << " ";
        }
        right--;
        for (int i = right; i >= left; --i) {
            cout << a[bottom][i] << " ";
        }
        bottom--;
        for (int i = bottom; i >= top; --i)
        {
            cout << a[i][left] << " ";
        }
        left++;
    }
    return(0);
}