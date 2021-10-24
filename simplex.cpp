#include <cstdio>
#include <vector>
#include <math.h>

#define M 25
#define N 50
#define EPS 1E-9
#define INF 9999

class Tableau
{
public:
    int rows, cols;
    int m, n;
    int min;
    double t[M + 1][N + 1] = {0};
    double B[M];
    double C[N];
    int s[M];

    Tableau()
    {
    }

    void read(char *filename)
    {
        FILE *fp = fopen(filename, "r");
        if (!fp)
        {
            printf("error file input\n");
            return;
        }
        read_rows_cols(fp);
        read_t(fp);
        m = rows;
        n = cols;
    }

    void read_rows_cols(FILE *fp)
    {
        fscanf(fp, "%d %d", &rows, &cols);
    }

    void read_t(FILE *fp)
    {
        int i, j;

        for (i = 0; i <= rows + 1; i++)
        {
            for (j = 0; j <= cols + 1; j++)
            {
                fscanf(fp, "%f", &t[i][j]);
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
        return true;
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
                double ratio = (t[h][n + 1] / t[h][l]);

                if ((fabs(ratio - min_ratio) < EPS && t[h][0] < min_tag) || ratio < min_ratio)
                {
                    k = h;
                    min_ratio = ratio;
                    min_tag = t[h][0];
                }
            }
        }
        return k;
    }

    void solve()
    {
        // is optimal t[m+1][k] >= 0, pt orice k
        int l, k, iteration;
        l = select_pivot_column();
        
        iteration = 0;
        show_table(iteration);
        while (1 <= l && l <= cols)
        {
            // `l` is the pivot column, entering basic variable on column l
            if (is_unbounded())
            {
                return -1;
            }

            k = select_pivot_row(l);
            if (!(1 <= k && k <= m))
            {
                return -2;
            }

            pivot_rule(k, l)

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
            t[k][0] = l; // update tag
            iteration++;
            show_table(iteration);
            l = select_pivot_row();
        }
    }

    void pivot_rule(int k, int l)
    {
        int i, j;
        for (i = 1; i <= m; ++i)
        {
            if (i == k)
                continue;
            for (j = 1; j <= n; ++j)
            {
                if (j == l)
                    continue;
                t[i][j] = (t[i][j] * t[k][l] - t[i][l] * t[k][j]) / t[k][l];
            }
        }
    }

    void show_table(int iteration)
    {
        printf("ITERATION %d \n", iteration);

        for (int i = 1; i <= m + 1; ++i)
        {
            if (i <= m)
                printf("x%d ", s[i]);
            else
                printf("z  ");

            for (int j = 1; j <= n + 1; ++j)
            {
                printf("%f ", t[i][j]);
            }
            printf("\n");
        }
        printf("----------------- \n");
    }
};

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
        iteration++;
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
    solve();

    return 0;
}
