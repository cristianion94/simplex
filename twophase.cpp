#include <cstdio>
#include <math.h>
#include <iostream> // std::cout
#include <fstream>  // std::ifstream
#include <vector>

using namespace std;

#define M 100
#define N 100
#define EPS 1E-9
#define INF 9999

int n, m, s; // rows, columns
double t[M][N];
vector<double> B;

void show(const vector<vector<double> > &A, const vector<double> &B, const vector<double> &C, double z);

inline int column_vector(const vector<vector<double> > &A, const vector<double> &C, int col)
{
    int count = 0;
    int row = -1;
    for (int r = 0; r < A.size(); r++)
    {
        if (A[r][col] > EPS)
        {
            count++;
            row = r;
        }
    }
    if (C[col] > -EPS && C[col] < EPS)
    {
        if (count == 1)
        {
            if (fabs(A[row][col] - 1.0) < EPS)
            {
                return row;
            }
            return -1;
        }
        return -1;
    }
    return -1;
}

void adjust_tableau(vector< vector< double > > &A, vector< double > &B, vector< double > &C, vector< int > &basic, double &z)
{
    int m = A.size();
    int n = C.size();
    
    for (int r = 0; r < m; r++)
    {
        int basic_col = basic[r];
        if (fabs(A[r][basic_col] - 1.0) > EPS)
        {
            double p = A[r][basic_col];
            for (int c = 0; c < n; c++)
                A[r][c] /= p;
            B[r] /= p;
        }
        if (fabs(C[basic_col]) > EPS)
        {
            double p = C[basic_col];
            for (int c = 0; c < n; c++)
                C[c] -= A[r][c] * p;
            z -= B[r] * p;
        }
    }
}

bool simplex(vector<vector<double> > &A, vector<double> &B, vector<double> &C, vector<int> &basic, double &z)
{
    // A tableau
    // C obj coeficitients
    // basic index
    // z objective value
    int m = A.size();
    int n = C.size();

    int iteration = 1;

    cout << "ITERATION : " << 0 << endl;
    show(A, B, C, z);

    while (1)
    {
        cout << "ITERATION : " << iteration << endl;
        int l = 0; // entering variable
        for (l = 0; l < n; l++)
        {
            if (C[l] < -EPS)
                break;
        }
        if (l == n) // optimum
            break;

        double minratio = INF;
        int minindex = INF;
        int k = -1;

        for (int h = 0; h < m; h++)
        {
            if (A[h][l] > EPS) // > 0
            {
                double ratio = (B[h] / A[h][l]);
                if ((fabs(ratio - minratio) < EPS && basic[h] < minindex) || ratio < minratio)
                {
                    k = h;
                    minratio = ratio;
                    minindex = basic[h];
                }
            }
        }

        if (k < 0) {
            show(A, B, C, z);
            return true; // unbounded
        }

        int leaving_variable = basic[k];
        basic[k] = l;
        cout << "LEAVING VARIABLE " << k << endl;
        cout << "ENTERING VARIABLE " << l << endl;


        for (int i = 0; i < m; i++)
        {
            if (i == k)
                continue;
            for (int j = 0; j < n; j++)
            {
                if (j == l)
                    continue;
                A[i][j] = (A[i][j] * A[k][l] - A[i][l] * A[k][j]) / A[k][l];
            }
            B[i] = (B[i] * A[k][l] - A[i][l] * B[k]) / A[k][l];
        }
        for (int j = 0; j < n; j++)
        {
            if (j == l)
                continue;
            C[j] = (C[j] * A[k][l] - C[l] * A[k][j]) / A[k][l];
        }
        z = (z * A[k][l] - C[l] * B[k]) / A[k][l];

        for (int i = 0; i < m; ++i)
        {
            if (i == k)
                continue;
            A[i][l] = 0;
        }
        for (int j = 0; j < n; ++j)
        {
            if (j == l)
                continue;
            A[k][j] /= A[k][l];
        }
        B[k] /= A[k][l];
        A[k][l] = 1;
        C[l] = 0;

        iteration++;

        show(A, B, C, z);
    }

    show(A, B, C, z);

    return false;
}

int twophase(const vector<vector<double> > &A, const vector<double> &B, const vector<double> &C, vector<double> &X, double &z)
{
    int m = A.size();
    int n = A[0].size();

    if (!m || m != B.size() || n != C.size())
    {
        exit(1);
    }

    if (X.size() != n) // solution must match number of variables
        X.resize(n);
    fill(X.begin(), X.end(), 0); // reset solution

    // A0, B0 - initial tableau
    vector<vector<double> > A0(m, vector<double>(n));
    vector<double> B0(m);
    for (int r = 0; r < m; r++)
        copy(A[r].begin(), A[r].end(), A0[r].begin());
    copy(B.begin(), B.end(), B0.begin());

    vector<bool> isbasic(n, false);
    vector<int> basis(m, -1);
    int num_basic = 0;

    // show(A, B, C, X, z);

    for (int c = 0; c < n; c++)
    {
        int r = column_vector(A, C, c);
        // cout << "column vector " << r << endl;
        if (r >= 0 && basis[r] < 0)
        {
            isbasic[c] = true;
            basis[r] = c;
            num_basic += 1;
        }
    }

    vector<vector<double> > A2(m, vector<double>(n));
    vector<double> B2(m);
    vector<double> C2(n);
    for (int r = 0; r < m; r++)
        copy(A0[r].begin(), A0[r].end(), A2[r].begin());
    copy(B0.begin(), B0.end(), B2.begin());

    for (int c = 0; c < n; c++)
        C2[c] = C[c];

    z = 0;

    cout << "(num_basic, m) = " << num_basic << ' ' << m << endl;

    if (num_basic < m) // trebuie adaugate variabile artificiale
    {
        int n1 = n;
        vector<vector<double> > A1(m, vector<double>(n));
        vector<double> B1(m);
        vector<double> C1(n, 0); // new objective vector for phase I

        for (int r = 0; r < m; r++) //
            copy(A0[r].begin(), A0[r].end(), A1[r].begin());
        copy(B0.begin(), B0.end(), B1.begin());

        for (int i = 0; i < m; i++)
        {
            if (basis[i] < 0)
            {
                for (int r = 0; r < m; r++)
                {
                    if (r == i)
                        A1[r].push_back(1);
                    else
                        A1[r].push_back(0);
                }
                C1.push_back(1);
                basis[i] = n1; // var. artificiale le introducem in baza
                n1++;
            }
        }
        C1.resize(n1, 1);

        adjust_tableau(A1, B1, C1, basis, z);

        cout << " Entering PHASE I. \n";
        bool unbounded = simplex(A1, B1, C1, basis, z);
        if (unbounded)
        {
            cout << "unbounded phase 1 \n";
            return -1;
        }

        bool feasible = (fabs(z) < EPS) ? true : false; 
        // daca z este close to zero, atunci pb e fezabila
        if (!feasible) {
            cout << "not feasible" << endl;
            return 1;
        }

        // copy for phase 2
        for (int r = 0; r < m; r++)
        {
            for (int c = 0; c < n; c++)
                A2[r][c] = A1[r][c];
            B2[r] = B1[r];
        }
    }

    cout << "\nEntering PHASE II. \n";
    adjust_tableau(A2, B2, C2, basis, z);
    bool unbounded = simplex(A2, B2, C2, basis, z);

    if (unbounded)
    {
        cout << "Unbounded phase 2 \n";
        return -2;
    }

    for (int r = 0; r < m; r++)
        X[basis[r]] = B2[r];

    return 0;
}

void show(const vector<vector<double> > &A, const vector<double> &B, const vector<double> &C, double z)
{
    int m = A.size();
    int n = A[0].size();

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << setprecision(2) << A[i][j] << '\t';
        }
        cout << "= " << B[i] << '\n';
    }
    for (int i = 0; i < n; i++)
    {
        cout << C[i] << '\t';
    }
    cout << setprecision(2) << z;

    cout << '\n';
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("error: filepath missing.\n");
        return -1;
    }

    ifstream fin(argv[1]);

    fin >> n; // n variables
    fin >> m; // m contraints

    vector<vector<double> > A(m, vector<double>(n));
    vector<double> B(m), C(n);

    cout << m << " constraints \n";
    cout << n << " variables \n";

    // read the contraints coeficients
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            fin >> A[i][j];
        }
    }

    // read the RHS
    for (int i = 0; i < m; ++i)
    {
        fin >> B[i];
    }
    // read the objective coeficients
    for (int i = 0; i < n; ++i)
    {
        fin >> C[i];
    }

    int s;
    for (int i = 0; i < m; ++i)
    {
        fin >> s;
        if (s != 0)
        {
            // introducem in A an one-column vector
            // with 1 at 'i' position and 0 in rest
            // create a new variable in A

            for (int j = 0; j < m; j++)
            {
                A[j].push_back(0);
            }
            int num_variables = A[0].size();
            A[i][num_variables - 1] = s;
            C.push_back(0);
        }
    }

    vector<double> X; // solution
    double z;         // objective value

    fin >> z; // citim si un z value obiectiv initial

    show(A, B, C, z);

    int ret = twophase(A, B, C, X, z);

    cout << "two phase ret " << ret << endl;
    cout << "optimal z = " << setprecision(2) << z << endl;
    cout << "x=( ";
    for (int i = 0; i < n; i++)
    {
        cout << X[i] << ' ';
    }
    cout << ")" << endl;

    cout << "DONE!" << endl
         << endl;

    return 0;
}
