#include <cstdio>

int n, m;
double t[1000][1000];
int s[1000];

// m columns
// n rows
// de implementat verificarea cu Epsilon la diferente
// te introdus etichetele `s` in tabloul `t` pe prima coloana pt simplificarea problemei


void read(char *file_name)
{
    FILE *file = fopen(file_name, "r");
    fscanf(file, "%d %d\n", &m, &n);

    for (int i = 1; i <= m + 1; ++i)
    {
        s[i] = i + (n-m);
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
    for (int l = 1; l <= n; ++l)
    {
        if (t[m + 1][l] < 0)
            return l;
    }
    return 0;
}


int select_pivot_row(int l)
{
    double minval = 99999;
    int k = 0;
    int mins = 99999;

    for (int h = 1; h <= m; ++h)
    {
        if (t[h][l] > 0) // selecting columns 
        {
            double val = (t[h][n + 1] / t[h][l]);
            if (val < minval || (val == minval && s[h] < mins))
            {
                k = h;
                minval = val;
                mins = s[h];
            }
        }
    }
    s[k] = l;
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
            printf("x%d ", s[i]);
        else printf("z  ");

        for (int j = 1; j <= n + 1; ++j)
        {
            printf("%f ", t[i][j]);
        }
        printf("\n");
    }
    
    printf("----------------- \n");
}

void solve()
{
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

    read(argv[1]);

    show_table(0);

    solve();

    return 0;
}