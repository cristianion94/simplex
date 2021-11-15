#include <cstdio>
#include <math.h>

#define M 25
#define N 50
#define EPS 1E-9
#define INF 9999

int n, m; // rows, columns
double t[1000][1000];


void read(char *file_name)
{
    FILE *file = fopen(file_name, "r");
    fscanf(file, "%d %d\n", &m, &n);

    for (int i = 1; i <= m + 1; ++i)
    {
        t[0][i] = i + (n-m); // punem variabilele slack ca basic
        for (int j = 1; j <= n + 1; ++j)
        {
            int x;
            fscanf(file, "%d", &x);
            t[i][j] = x * 1.0;
        }
    }
}


bool is_unbounded(int l)
{
    for (int h = 1; h <= m; ++h)
    {
        if (t[h][l] > 0)
            return false;
    }
    return true; // is unbounded
}


int select_pivot_column()
{
    // select the most negative reduced cost
    // t[m+1] reduced cost
    for (int l = 1; l <= n; ++l)
    {
        if (t[m + 1][l] < 0)
            return l;
    }
    return 0;
}


int select_pivot_row(int l)
{
    double min_ratio = INF;
    int min_tag = INF;
    int k = -1;

    for (int h = 1; h <= m; ++h)
    {
        if (t[h][l] > 0)
        {
            double ratio = (t[h][n + 1] / t[h][l]); // RHS / t[h][l]s

            if ((fabs(ratio - min_ratio) < EPS && t[h][0] < min_tag) || ratio < min_ratio) // Bland's rule
            {
                k = h; // k is the selected row
                min_ratio = ratio;
                min_tag = t[h][0];
            }
        }
    }
    
    t[k][0] = l; // enter basis

    return k;
}

void pivot(int l, int k)
{
    for (int i = 1; i <= m + 1; ++i)
    {
        if (i == k)
            continue;
        for (int j = 1; j <= n + 1; ++j)
        {
            if (j == l)
                continue;

            // pivoting rule
            // i-th row, j-th column
            // i, l - linia pivotului
            // k, j - coloana pivotului
            t[i][j] = (t[i][j] * t[k][l] - t[i][l] * t[k][j]) / t[k][l];
        }
    }

    for (int i = 1; i <= m + 1; ++i)
    {
        if (i == k)
            continue;
        t[i][l] = 0;
    }

    for (int j = 1; j <= n + 1; ++j)
    {
        if (j == l)
            continue;
        t[k][j] = t[k][j] / t[k][l];
    }

    t[k][l] = 1;
}

void show_table(int iteration)
{
    printf("ITERATION %d \n", iteration);

    for (int i = 1; i <= m + 1; ++i)
    {
        if(i <= m)
            printf("x%d ", t[i][0]);
        else printf("z  ");

        for (int j = 1; j <= n + 1; ++j)
        {
            printf("%f ", t[i][j]);
        }
        printf("\n");
    }
    
    printf("----------------- \n");
}

void solve_simplex()
{
    show_table(0);

    int l = select_pivot_column();
    int iteration = 1;

    while (1 <= l && l <= n)
    {
        if (is_unbounded(l))
        {
            printf("unbounded\n");
            return;
        }

        int k = select_pivot_row(l);
        printf("PIVOT t(l=%d, k=%d)=%f \n", l, k, t[l][k]);
        pivot(l, k);

        show_table(iteration);

        l = select_pivot_column();
        iteration ++;
    }

    printf("z = %.1f\n", -t[m + 1][n + 1]);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("error: filepath missing.\n");
        return -1;
    }

    read(argv[1]); // citim problema ca matrice
    solve_simplex();

    return 0;
}
